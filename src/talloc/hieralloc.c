/*
 * Similar core function to LGPL licensed talloc from Samba
 */

#include "hieralloc.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct hieralloc_header
{
	unsigned beginMagic;
	struct hieralloc_header * parent;
	struct hieralloc_header * nextSibling, * prevSibling;
	struct hieralloc_header * child;
	const char * name;
	unsigned size, childCount, refCount;
	int (* destructor)(void *);
	unsigned endMagic;
} hieralloc_header_t;

#define BEGIN_MAGIC() (13377331)
#define END_MAGIC(header) ((unsigned)((const hieralloc_header_t *)header + 1) % 0x10000 | 0x13370000)

static hieralloc_header_t global_header = {0, 0, 0, 0, 0, "hieralloc_global_header", 0, 0 ,1, 0, 0};

// Returns 1 if it's a valid header
static inline int check_header(const hieralloc_header_t * header)
{
	if (&global_header == header)
		return 1;
	assert(BEGIN_MAGIC() == header->beginMagic);
	assert(END_MAGIC(header) == header->endMagic);
	return 1;
}

static inline hieralloc_header_t * get_header(const void *ptr)
{
	hieralloc_header_t * header = (hieralloc_header_t *)(ptr) - 1;
	check_header(header);
	return header;
}

// attach to parent and siblings
static inline void add_to_parent(hieralloc_header_t * parent, hieralloc_header_t * header)
{
	assert(NULL == header->parent);
	assert(NULL == header->prevSibling);
	assert(NULL == header->nextSibling);

	if (parent->child)
		parent->child->prevSibling = header;
	header->nextSibling = parent->child;
	header->prevSibling = NULL;
	header->parent = parent;
	parent->child = header;
	parent->childCount++;
}

// detach from parent and siblings
static inline void remove_from_parent(hieralloc_header_t * header)
{
	hieralloc_header_t * parent = header->parent;
	hieralloc_header_t * sibling = header->prevSibling;
	if (sibling)
	{
		assert(sibling->nextSibling == header);
		sibling->nextSibling = header->nextSibling;
		if (header->nextSibling)
			header->nextSibling->prevSibling = sibling;
		header->prevSibling = NULL;
		header->nextSibling = NULL;
	}
	else
	{
		parent->child = header->nextSibling;
		if (header->nextSibling)
			header->nextSibling->prevSibling = NULL;
		header->nextSibling = NULL;
	}
	header->parent = NULL;
	parent->childCount--;
}

// allocate memory and attach to parent context and siblings
void * _hieralloc_allocate(const void * context, size_t size, const char * name)
{
	hieralloc_header_t * ptr = malloc(size + sizeof(hieralloc_header_t));
	assert(ptr);
	memset(ptr, 0, sizeof(*ptr));
	ptr->beginMagic = BEGIN_MAGIC();
	ptr->name = name;
	ptr->size = size;
	ptr->childCount = 0;
	ptr->refCount = 1;
	ptr->endMagic = END_MAGIC(ptr);

	hieralloc_header_t * parent = NULL;
	if (!context)
		parent = &global_header;
	else
		parent = get_header(context);
	check_header(parent);

	add_to_parent(parent, ptr);
	return ptr + 1;
}

// (re)allocate memory and attach to parent context and siblings
static inline void * _hieralloc_reallocate(const void * context, void * ptr, size_t size, const char * name)
{
	if (NULL == ptr)
		return _hieralloc_allocate(context, size, name);

	int reparented = 0;
	if (NULL == context)
	{
		context = &global_header + 1;
		reparented = 1;
	}

	hieralloc_header_t * header = get_header(ptr);
	hieralloc_header_t * parent = header->parent;

	if (get_header(context) != get_header(ptr)->parent)
	{
		remove_from_parent(header);
		parent = get_header(context);
		add_to_parent(parent, header);
	}

	hieralloc_header_t * sibling = header->prevSibling;

	ptr = realloc(header, size + sizeof(hieralloc_header_t));
	assert(ptr);
	((hieralloc_header_t *)ptr)->size = size;
	if (ptr == header)
		return header + 1;

	header = ptr;
	header->beginMagic = BEGIN_MAGIC();
	header->endMagic = END_MAGIC(header);
	if (sibling)
		sibling->nextSibling = header;
	else
		parent->child = header;

	hieralloc_header_t * child = header->child;
	while (child)
	{
		child->parent = header;
		child = child->nextSibling;
	}

	return header + 1;
}

// calls destructor if set, and frees children.
// if destructor returns -1, then do nothing and return -1.
int _hieralloc_free(void * ptr, const char * location)
{
	if (!ptr)
		return 0;
	hieralloc_header_t * header = get_header(ptr);
	header->refCount--;
	if (header->refCount > 0)
		return -1;

	if (header->destructor)
		if (header->destructor(ptr))
			return -1;

	int ret = 0;
	//* TODO: implement reference and steal first
	hieralloc_header_t * child = header->child;
	while (child)
	{
		hieralloc_header_t * current = child;
		child = child->nextSibling;
		if (_hieralloc_free(current + 1, location))
		{
			ret = 1;
			remove_from_parent(current);
			add_to_parent(header->parent, current);
		}
	}
	//*/
	
	if (ret)
		return 1;
		
	remove_from_parent(header);
	memset(header, 0xfe, header->size + sizeof(*header));
	free(header);
	return 0;
}

// not implemented from _talloc_reference_loc
void * _hieralloc_reference_loc(const void * ctx, const void * ptr, const char * location)
{
	if (!ptr)
		return NULL;
		
	get_header(ptr)->refCount++;
	return ptr;
	
	/*
	hieralloc_header_t * newParent = get_header(ctx);
	hieralloc_header_t * reference = _hieralloc_allocate(ctx, 0, location);
	add_to_parent(newParent, reference);
	
	hieralloc_header_t * header = get_header(ptr);
	reference->child = header;
	reference->childCount = 1;
	header->parent = reference;
	header->refCount++;
	return ptr;
	//*/
}

// not implemented from talloc_unlink
int hieralloc_unlink(const void * ctx, void *ptr)
{
	if (!ptr)
		return -1;
	//assert(get_header(ptr)->refCount > 0);
	//get_header(ptr)->refCount--;
	return 0;
}

// moves allocation to new parent context; maintain children but update siblings
void * _hieralloc_steal_loc(const void * new_ctx, const void * ptr, const char * location)
{
	hieralloc_header_t * header = get_header(ptr);
	remove_from_parent(header);
	add_to_parent(get_header(new_ctx), header);
	return ptr;
}

// creates 0 allocation to be used as parent context
void * hieralloc_init(const char * name)
{
	return _hieralloc_allocate(NULL, 0, name);
}

void * _hieralloc_array(const void * ctx, unsigned elemSize, unsigned count, const char * name)
{
	return _hieralloc_allocate(ctx, elemSize * count, name);
}

void * _hieralloc_realloc_array(const void * ctx, void * ptr, unsigned elemSize, unsigned count, const char * name)
{
	return _hieralloc_reallocate(ctx, ptr, elemSize * count, name);
}

// returns global context
void * hieralloc_autofree_context()
{
	return &global_header + 1;
}

// sets destructor to be called before freeing; dctor return -1 aborts free
void _hieralloc_set_destructor(const void * ptr, int (* destructor)(void *))
{
	get_header(ptr)->destructor = destructor;
}

// gets parent context of allocated memory
void * hieralloc_parent(const void * ptr)
{
	const hieralloc_header_t * header = get_header(ptr);
	return header->parent + 1;
}

// allocate and zero memory
void * _hieralloc_zero(const void * ctx, unsigned size, const char * name)
{
	void *p = _hieralloc_allocate(ctx, size, name);
	if (p)
		memset(p, 0, size);
	return p;
}

static inline char * __hieralloc_strlendup(const void * ctx, const char * str, unsigned len)
{
	char * ret = (char *)_hieralloc_allocate(ctx, len + 1, str);
	if (!ret)
		return NULL;
	memcpy(ret, str, len);
	ret[len] = 0;
	return ret;
}

// allocate and copy
char * hieralloc_strdup(const void * ctx, const char * str)
{
	if (!str)
		return NULL;
	return __hieralloc_strlendup(ctx, str, strlen(str));
}

// allocate and copy
char * hieralloc_strndup(const void * ctx, const char * str, unsigned len)
{
	if (!str)
		return NULL;
	return __hieralloc_strlendup(ctx, str, strnlen(str, len));
}

static inline char * __hieralloc_strlendup_append(char * str, unsigned len,
        const char * append, unsigned appendLen)
{
	char * ret = _hieralloc_reallocate(NULL, str, sizeof(char) * (len + appendLen + 1), str);
	if (!ret)
		return NULL;
	memcpy(&ret[len], append, appendLen);
	ret[len + appendLen] = 0;
	get_header(ret)->name = ret;
	return ret;
}

// reallocate and append
char * hieralloc_strdup_append(char * str, const char * append)
{
	if (!str)
		return hieralloc_strdup(NULL, append);
	if (!append)
		return str;
	return __hieralloc_strlendup_append(str, strlen(str), append, strlen(append));
}

// reallocate and append
char * hieralloc_strndup_append(char * str, const char * append, unsigned len)
{
	if (!str)
		return hieralloc_strdup(NULL, append);
	if (!append)
		return str;
	return __hieralloc_strlendup_append(str, strlen(str), append, strnlen(append, len));
}

// allocate and vsprintf
char * hieralloc_vasprintf(const void * ctx, const char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);
	char c = 0;
	int len = vsnprintf(&c, 1, fmt, va2); // count how many chars would be printed
	va_end(va2);

	assert(len >= 0); // some vsnprintf may return -1
	if (len < 0)
		return NULL;

	char * ret = (char *)_hieralloc_allocate(ctx, len + 1, fmt);
	if (!ret)
		return NULL;

	va_copy(va2, va);
	vsnprintf(ret, len + 1, fmt, va2);
	va_end(va2);

	get_header(ret)->name = ret;
	return ret;
}

// allocate and sprintf
char * hieralloc_asprintf(const void * ctx, const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char * ret = hieralloc_vasprintf(ctx, fmt, va);
	va_end(va);
	return ret;
}

static inline char * __hieralloc_vaslenprintf_append(char * str, unsigned len,
        const char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);
	char c = 0;
	int appendLen = vsnprintf(&c, 1, fmt, va2); // count how many chars would be printed
	va_end(va2);

	assert(appendLen >= 0); // some vsnprintf may return -1
	if (appendLen < 0)
		return str;
	str = _hieralloc_reallocate(NULL, str, sizeof(char) * (len + appendLen + 1), str);
	if (!str)
		return NULL;

	va_copy(va2, va);
	vsnprintf(str + len, appendLen + 1, fmt, va2);
	va_end(va2);

	get_header(str)->name = str;
	return str;
}

// reallocate and append vsprintf
char * hieralloc_vasprintf_append(char * str, const char * fmt, va_list va)
{
	if (!str)
		return hieralloc_vasprintf(NULL, fmt, va);
	return __hieralloc_vaslenprintf_append(str, strlen(str), fmt, va);
}

// reallocate and append sprintf
char * hieralloc_asprintf_append(char * str, const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	str = hieralloc_vasprintf_append(str, fmt, va);
	va_end(va);
	return str;
}

static void _hieralloc_report(const hieralloc_header_t * header, FILE * file, unsigned tab)
{
	unsigned i = 0;
	for (i = 0; i < tab; i++)
		fputc(' ', file);
	fprintf(file, "%p: child=%d ref=%d size=%d name='%.256s' \n", header + 1,
	        header->childCount, header->refCount, header->size, header->name);
	const hieralloc_header_t * child = header->child;
	while (child)
	{
		_hieralloc_report(child, file, tab + 2);
		child = child->nextSibling;
	}

}

// report self and child allocations
void hieralloc_report(const void * ptr, FILE * file)
{
	if (NULL == ptr)
		ptr = &global_header + 1;
	fputs("hieralloc_report: \n", file);
	_hieralloc_report(get_header(ptr), file, 0);
}

static void _hieralloc_report_brief(const hieralloc_header_t * header, FILE * file, unsigned * data)
{
	data[0]++;
	data[1] += header->size;
	data[2] += header->childCount;
	data[3] += header->refCount;
	const hieralloc_header_t * child = header->child;
	while (child)
	{
		_hieralloc_report_brief(child, file, data);
		child = child->nextSibling;
	}
}

void hieralloc_report_brief(const void * ptr, FILE * file)
{
	if (NULL == ptr)
		ptr = &global_header + 1;
	unsigned data [4] = {0};
	_hieralloc_report_brief(get_header(ptr), file, data);
	fprintf(file, "hieralloc_report total: count=%d size=%d child=%d ref=%d \n",
		data[0], data[1], data[2], data[3]);
}
