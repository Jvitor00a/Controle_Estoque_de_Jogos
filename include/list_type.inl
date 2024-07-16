#include "macro_kit.h"
#include <stddef.h>
#include <stdlib.h>

#ifndef INCLUDE_LIST_TYPE_INL
#error This file should only be included from headers \
that define INCLUDE_LIST_TYPE_INL
#endif

#ifndef LIST_ELEMENT_TYPE
#error Need a definition for LIST_ELEMENT_TYPE
#endif

/* list_type.inl

    Defines and implements a generic list-of-T container
    for T the current values of the macros:

    - LIST_ELEMENT_TYPE:
        - must have a definition = the datatype (or typedef alias) for \
        which a list container is required.

    - LIST_ELEMENT_COPY_INITOR:
        - If undefined, then LIST_ELEMENT_TYPE is assumed to be copy-
        initializable by the assignment operator. Otherwise must be defined
        as the name of a copy initialization function having a prototype of
        the form:

        LIST_ELEMENT_TYPE * copy_initor_name(LIST_ELEMENT_TYPE *pdest,
                                            LIST_ELEMENT_TYPE *psrc);

        that will attempt to copy the LIST_ELEMENT_TYPE at `psrc` into the
        uncommitted memory at `pdest`, returning `pdest` on success and NULL
        on failure.

        N.B. This file itself defines the copy initializor for the list-type
        that it generates.

    - LIST_ELEMENT_DISPOSE
        If undefined, then LIST_ELEMENT_TYPE is assumed to need no
        destruction. Otherwise the name of a destructor function having a
        protoype of the form:

        void dtor_name(LIST_ELEMENT_TYPE pt*);

        that appropriately destroys the LIST_ELEMENT_TYPE at `pt`.

        N.B. This file itself defines the destructor for the list-type that
        it generates.
*/

/* Define the names of the list-type to generate,
    e.g. ListInt, ListFloat
*/
#define LIST_TYPE CAT2(Lista, LIST_ELEMENT_TYPE)

/* Define the function-names of the LIST_TYPE API.
    Each of the API macros LIST_XXXX generates a function name in
    which LIST becomes the value of LIST_TYPE and XXXX becomes TitleCase,
    e.g ListIntNew
*/
#define LIST_NEW CAT2(LIST_TYPE, Nova)
#define LIST_NODE CAT2(LIST_TYPE, Nó)
#define LIST_DISPOSE CAT2(LIST_TYPE, Descartar)
#define LIST_COPY_INIT CAT2(LIST_TYPE, CopiaInicial)
#define LIST_COPY CAT2(LIST_TYPE, Copia)
#define LIST_BEGIN CAT2(LIST_TYPE, Inicio)
#define LIST_END CAT2(LIST_TYPE, Fim)
#define LIST_SIZE CAT2(LIST_TYPE, Tamanho)
#define LIST_INSERT_BEFORE CAT3(LIST_TYPE, Inserir, Antes)
#define LIST_DELETE_BEFORE CAT3(LIST_TYPE, Deletar, Antes)
#define LIST_PUSH_BACK CAT3(LIST_TYPE, Empurrar, Tras)
#define LIST_PUSH_FRONT CAT3(LIST_TYPE, Empurrar, Frente)
#define LIST_POP_BACK CAT3(LIST_TYPE, Retirar, Tras)
#define LIST_POP_FRONT CAT3(LIST_TYPE, Retirar, Frente)
#define LIST_GET_AT_POSITION CAT2(CAT3(LIST_TYPE, Obter, Em), Posicao)
#define LIST_NODE_GET CAT2(LIST_NODE, Obter)
#define LIST_NODE_NEXT CAT2(LIST_NODE, Proximo)
#define LIST_NODE_PREV CAT2(LIST_NODE, Anterior)

/* Define the name of the structure used to implement a LIST_TYPE.
    This structure is not exposed to user code.
*/
#define LIST_STRUCT CAT2(LIST_TYPE, Struct)

/* Define the name of the structure used to implement a node of a LIST_TYPE.
    This structure is not exposed to user code.
*/
#define LIST_NODE_STRUCT CAT2(LIST_NODE, Struct)

/* The LIST_TYPE API... */

// Define the abstract list type
typedef struct LIST_STRUCT *LIST_TYPE;

// Define the abstract list node type
typedef struct LIST_NODE_STRUCT *LIST_NODE;

/* Return a pointer to the LIST_ELEMENT_TYPE in a LIST_NODE `node`,
    or NULL if `node` is null
*/
extern LIST_ELEMENT_TYPE *LIST_NODE_GET(LIST_NODE node);

/* Return the LIST_NODE successor of a LIST_NODE `node`,
    or NULL if `node` is null.
*/
extern LIST_NODE LIST_NODE_NEXT(LIST_NODE node);

/* Return the LIST_NODE predecessor of a LIST_NODE `node`,
    or NULL if `node` is null.
*/
extern LIST_NODE LIST_NODE_PREV(LIST_NODE node);

/* Create a new LIST_TYPE optionally initialized with elements copied from
    `start` and until `end`.

    If `end` is null it is assumed == `start` + 1.

    If `start` is not NULL then elements will be appended to the
    LIST_TYPE until `end` or until an element cannot be successfully copied.
    The size of the LIST_TYPE will be the number of successfully copied
    elements.
*/
extern LIST_TYPE LIST_NEW(LIST_ELEMENT_TYPE *start, LIST_ELEMENT_TYPE *end);

/* Dispose of a LIST_TYPE
    If the pointer to LIST_TYPE `plist` is not null and addresses
    a non-null LIST_TYPE then the LIST_TYPE it addresses is
    destroyed and set NULL.
*/
extern void LIST_DISPOSE(LIST_TYPE *plist);

/* Copy the LIST_TYPE at `psrc` into the LIST_TYPE-sized region at `pdest`,
    returning `pdest` on success, else NULL.

    If copying is unsuccessful the LIST_TYPE-sized region at `pdest is
    unchanged.
*/
extern LIST_TYPE *LIST_COPY_INIT(LIST_TYPE *pdest, LIST_TYPE *psrc);

/* Return a copy of the LIST_TYPE `src`, or NULL if `src` cannot be
    successfully copied.
*/
extern LIST_TYPE LIST_COPY(LIST_TYPE src);

/* Return a LIST_NODE referring to the  start of the
    LIST_TYPE `list`, or NULL if `list` is null.
*/
extern LIST_NODE LIST_BEGIN(LIST_TYPE list);

/* Return a LIST_NODE referring to the end of the
    LIST_TYPE `list`, or NULL if `list` is null.
*/
extern LIST_NODE LIST_END(LIST_TYPE list);

/* Return the number of LIST_ELEMENT_TYPEs in the LIST_TYPE `list`
    or 0 if `list` is null.
*/
extern size_t LIST_SIZE(LIST_TYPE list);

/* Add an element to the back of the list.
 * Parameters:
 * - list: The list to which the element should be added.
 * - element: A pointer to the element to add.
 */
extern void LIST_PUSH_BACK(LIST_TYPE list, LIST_ELEMENT_TYPE *element);

/* Add an element to the front of the list.
 * Parameters:
 * - list: The list to which the element should be added.
 * - element: A pointer to the element to add.
 */
extern void LIST_PUSH_FRONT(LIST_TYPE list, LIST_ELEMENT_TYPE *element);

/* Remove an element from the back of the list.
 * Parameters:
 * - list: The list from which the element should be removed.
 */
extern void LIST_POP_BACK(LIST_TYPE list);

/* Remove an element from the front of the list.
 * Parameters:
 * - list: The list from which the element should be removed.
 */
extern void LIST_POP_FRONT(LIST_TYPE list);

/* Return the LIST_NODE at the given position in the list, counting from the
 * beginning. Parameters:
 * - list: The list from which the node should be retrieved.
 * - position: The position (0-based index) of the node to retrieve.
 * Returns:
 * - The LIST_NODE at the given position, or NULL if the position is out of
 * bounds.
 */
extern LIST_NODE LIST_GET_AT_POSITION(LIST_TYPE list, size_t position);

/* Insert an element before a given node in the list.
 * Parameters:
 * - list: The list in which the element should be inserted.
 * - node: The node before which the element should be inserted.
 * - element: A pointer to the element to insert.
 */
extern void LIST_INSERT_BEFORE(LIST_TYPE list, LIST_NODE node, LIST_ELEMENT_TYPE *element);

/* Delete the element before a given node in the list.
 * Parameters:
 * - list: The list from which the element should be deleted.
 * - node: The node before which the element should be deleted.
 */
extern void LIST_DELETE_BEFORE(LIST_TYPE list, LIST_NODE node);

/* If LIST_IMPLEMENT is defined then the implementation of LIST_TYPE is
    compiled, otherwise skipped. #define LIST_IMPLEMENT to include this
    file in the .c file that implements LIST_TYPE. Leave it undefined
    to include this file in the .h file that defines the LIST_TYPE API.
*/
#ifdef LIST_IMPLEMENT
// Implementation code now included.

// Standard library #includes...?

// The heap structure of a list node
struct LIST_NODE_STRUCT
{
    struct LIST_NODE_STRUCT *_next;
    struct LIST_NODE_STRUCT *_prev;
    LIST_ELEMENT_TYPE _data[1];
};

// The heap structure of a LIST_TYPE
struct LIST_STRUCT
{
    size_t _size;
    struct LIST_NODE_STRUCT *_anchor;
};

extern LIST_ELEMENT_TYPE *LIST_NODE_GET(LIST_NODE node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->_data;
}

/* Return the LIST_NODE successor of a LIST_NODE `node`,
    or NULL if `node` is null.
*/
extern LIST_NODE LIST_NODE_NEXT(LIST_NODE node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->_next;
}

/* Return the LIST_NODE predecessor of a LIST_NODE `node`,
    or NULL if `node` is null.
*/
extern LIST_NODE LIST_NODE_PREV(LIST_NODE node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->_prev;
}

/* Create a new LIST_TYPE optionally initialized with elements copied from
    `start` and until `end`.
*/
extern LIST_TYPE LIST_NEW(LIST_ELEMENT_TYPE *start, LIST_ELEMENT_TYPE *end)
{
    LIST_TYPE list = (LIST_TYPE)malloc(sizeof(struct LIST_STRUCT));
    if (!list)
    {
        return NULL;
    }
    list->_size = 0;
    list->_anchor = NULL;

    if (start && end)
    {
        for (LIST_ELEMENT_TYPE *it = start; it != end; ++it)
        {
            LIST_ELEMENT_TYPE *new_element = malloc(sizeof(LIST_ELEMENT_TYPE));
            if (!new_element)
            {
                LIST_DISPOSE(&list);
                return NULL;
            }
            memcpy(new_element, it, sizeof(LIST_ELEMENT_TYPE));
            LIST_PUSH_BACK(list, new_element);
        }
    }
    return list;
}

/* Dispose of a LIST_TYPE */
extern void LIST_DISPOSE(LIST_TYPE *plist)
{
    if (plist && *plist)
    {
        LIST_NODE current = (*plist)->_anchor;
        while (current)
        {
            LIST_NODE next = current->_next;
            free(current);
            current = next;
        }
        free(*plist);
        *plist = NULL;
    }
}

/* Copy the LIST_TYPE at `psrc` into the LIST_TYPE-sized region at `pdest`,
    returning `pdest` on success, else NULL.
*/
extern LIST_TYPE *LIST_COPY_INIT(LIST_TYPE *pdest, LIST_TYPE *psrc)
{
    if (!pdest || !psrc)
    {
        return NULL;
    }

    *pdest = LIST_NEW(NULL, NULL);
    if (!*pdest)
    {
        return NULL;
    }

    LIST_NODE current = LIST_BEGIN(*psrc);
    while (current)
    {
        LIST_ELEMENT_TYPE *new_element = malloc(sizeof(LIST_ELEMENT_TYPE));
        if (!new_element)
        {
            LIST_DISPOSE(pdest);
            return NULL;
        }
        memcpy(new_element, LIST_NODE_GET(current), sizeof(LIST_ELEMENT_TYPE));
        LIST_PUSH_BACK(*pdest, new_element);
        current = LIST_NODE_NEXT(current);
    }
    return pdest;
}

/* Return a copy of the LIST_TYPE `src`, or NULL if `src` cannot be
    successfully copied.
*/
extern LIST_TYPE LIST_COPY(LIST_TYPE src)
{
    LIST_TYPE copy = NULL;
    if (LIST_COPY_INIT(&copy, &src) == NULL)
    {
        return NULL;
    }
    return copy;
}

/* Return a LIST_NODE referring to the start of the
    LIST_TYPE `list`, or NULL if `list` is null.
*/
extern LIST_NODE LIST_BEGIN(LIST_TYPE list)
{
    if (list == NULL)
    {
        return NULL;
    }
    return list->_anchor;
}

/* Return a LIST_NODE referring to the end of the
    LIST_TYPE `list`, or NULL if `list` is null.
*/
extern LIST_NODE LIST_END(LIST_TYPE list)
{
    if (list == NULL)
    {
        return NULL;
    }
    LIST_NODE current = list->_anchor;
    while (current && current->_next)
    {
        current = current->_next;
    }
    return current;
}

/* Return the number of LIST_ELEMENT_TYPEs in the LIST_TYPE `list`
    or 0 if `list` is null.
*/
extern size_t LIST_SIZE(LIST_TYPE list)
{
    if (list == NULL)
    {
        return 0;
    }
    return list->_size;
}

// Implementation of LIST_PUSH_BACK
extern void LIST_PUSH_BACK(LIST_TYPE list, LIST_ELEMENT_TYPE *element)
{
    if (!list || !element)
    {
        return;
    }
    LIST_NODE new_node = (LIST_NODE)malloc(sizeof(struct LIST_NODE_STRUCT));
    if (!new_node)
    {
        return;
    }
    new_node->_data[0] = *element;
    new_node->_next = NULL;

    if (list->_anchor == NULL)
    {
        new_node->_prev = NULL;
        list->_anchor = new_node;
    }
    else
    {
        LIST_NODE last = LIST_END(list);
        last->_next = new_node;
        new_node->_prev = last;
    }
    list->_size++;
}

extern void LIST_PUSH_FRONT(LIST_TYPE list, LIST_ELEMENT_TYPE *element)
{
    if (!list || !element)
    {
        return;
    }
    LIST_NODE new_node = (LIST_NODE)malloc(sizeof(struct LIST_NODE_STRUCT));
    if (!new_node)
    {
        return;
    }
    new_node->_data[0] = *element;
    new_node->_prev = NULL;

    if (list->_anchor == NULL)
    {
        new_node->_next = NULL;
        list->_anchor = new_node;
    }
    else
    {
        new_node->_next = list->_anchor;
        list->_anchor->_prev = new_node;
        list->_anchor = new_node;
    }
    list->_size++;
}

extern void LIST_POP_BACK(LIST_TYPE list)
{
    if (!list || list->_size == 0)
    {
        return;
    }
    LIST_NODE last = LIST_END(list);
    if (last->_prev)
    {
        last->_prev->_next = NULL;
    }
    else
    {
        list->_anchor = NULL;
    }
    free(last);
    list->_size--;
}

extern void LIST_POP_FRONT(LIST_TYPE list)
{
    if (!list || list->_size == 0)
    {
        return;
    }
    LIST_NODE first = list->_anchor;
    if (first->_next)
    {
        first->_next->_prev = NULL;
    }
    list->_anchor = first->_next;
    free(first);
    list->_size--;
}

extern LIST_NODE LIST_GET_AT_POSITION(LIST_TYPE list, size_t position)
{
    if (!list || position >= list->_size)
    {
        return NULL;
    }

    LIST_NODE current = LIST_BEGIN(list);
    for (size_t i = 0; i < position; ++i)
    {
        current = LIST_NODE_NEXT(current);
    }

    return current;
}

extern void LIST_INSERT_BEFORE(LIST_TYPE list, LIST_NODE node, LIST_ELEMENT_TYPE *element)
{
    if (!list || !node || !element)
    {
        return;
    }
    LIST_NODE new_node = (LIST_NODE)malloc(sizeof(struct LIST_NODE_STRUCT));
    if (!new_node)
    {
        return;
    }
    new_node->_data[0] = *element;
    new_node->_next = node;
    new_node->_prev = node->_prev;

    if (node->_prev)
    {
        node->_prev->_next = new_node;
    }
    else
    {
        list->_anchor = new_node;
    }
    node->_prev = new_node;
    list->_size++;
}

extern void LIST_DELETE_BEFORE(LIST_TYPE list, LIST_NODE node)
{
    if (!list || !node || !node->_prev)
    {
        return;
    }
    LIST_NODE to_delete = node->_prev;
    node->_prev = to_delete->_prev;

    if (to_delete->_prev)
    {
        to_delete->_prev->_next = node;
    }
    else
    {
        list->_anchor = node;
    }
    free(to_delete);
    list->_size--;
}

/*  Undefine LIST_IMPLEMENT whenever it was defined.
    Should never fall through.
*/
#undef LIST_IMPLEMENT

#endif // LIST_IMPLEMENT

/*  Always undefine all the LIST_TYPE parameters.
    Should never fall through.
*/
#undef LIST_ELEMENT_TYPE
#undef LIST_ELEMENT_COPY_INITOR
#undef LIST_ELEMENT_DISPOSE
/* Also undefine the "I really meant to include this" flag. */

#undef INCLUDE_LIST_TYPE_INL