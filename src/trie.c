#include "../include/trie.h"

static void queue_init(queue_t *q)
{
    q->front = NULL;
    q->rear = NULL;
}
static void queue_push(queue_t *q, trie_node_t *trie_node)
{
    queue_node_t *new_node = malloc(sizeof(queue_node_t));
    if (!new_node)
    {
        fprintf(stderr, "malloc basarisiz (queue_push)\n");
        return;
    }
    new_node->trie_node = trie_node;
    new_node->next = NULL;

    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
    }
    else
    {
        q->rear->next = new_node;
        q->rear = new_node;
    }
}
static trie_node_t *queue_pop(queue_t *q)
{
    if (q->front == NULL)
        return NULL;
    
    queue_node_t *temp = q->front;
    trie_node_t *result = temp->trie_node;
    q->front = q->front->next;
    if(q->front == NULL)
        q->rear = NULL;
    free(temp);
    return result;
}   
static int queue_is_empty(queue_t *q)
{
    return q->front == NULL;
}

static trie_node_t *create_node(void)
{
    trie_node_t *node = malloc(sizeof(trie_node_t));
    if (!node)
    {
        fprintf(stderr, "malloc basarisiz (create_node)\n");
        return NULL;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;

    node->fail = NULL;
    node->is_end_of_pattern = 0;
    node->output_count = 0;
    node->pattern_name[0] = '\0';

    return node;
}
int load_patterns_from_file(trie_node_t *root, const char *filepath)
{
    FILE *fp = fopen(filepath, "r");
    if (!fp)
    {
        perror("fopen (load_patterns_from_file)");
        return -1;
    }

    char line[512];
    int loaded_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0)
            continue;

        char *comma = strchr(line, ',');
        if (comma == NULL)
        {
            fprintf(stderr, "Gecersiz satir formati, atlaniyor: %s\n", line);
            continue;
        }

        *comma = '\0';
        const char *hex_part = line;
        const char *name_part = comma + 1;

        unsigned char pattern_bytes[MAX_PATTERN_LEN];
        int byte_count = hex_to_bytes(hex_part, pattern_bytes, MAX_PATTERN_LEN - 1);

        if (byte_count <= 0)
        {
            fprintf(stderr, "Gecersiz hex pattern, atlaniyor: %s\n", hex_part);
            continue;
        }

        pattern_bytes[byte_count] = '\0'; 

        trie_insert(root, (const char *)pattern_bytes, name_part);
        loaded_count++;
    }

    fclose(fp);
    printf("%d pattern yuklendi.\n", loaded_count);
    return loaded_count;
}
void trie_insert(trie_node_t *root, const char *pattern, const char *malware_name)
{
    trie_node_t *current = root;
    for (int i = 0; pattern[i] != '\0'; i++)
    {
        unsigned char index = (unsigned char)pattern[i];
        if (current->children[index] == NULL)
        {
            current->children[index] = create_node();
            if (current->children[index] == NULL)
            {
                fprintf(stderr, "Node olusturulamadi (trie_insert)\n");
                return;
            }
        }
        current = current->children[index];
    }
    current->is_end_of_pattern = 1;
    current->output_count++;
    strncpy(current->pattern_name, malware_name,sizeof(current->pattern_name) - 1);
    current->pattern_name[sizeof(current->pattern_name) -1] = '\0';
}
void build_failure_links(trie_node_t *root)
{
    queue_t q;
    queue_init(&q);
    
    for(int byte = 0; byte < ALPHABET_SIZE; byte++)
    {
        if(root->children[byte])
        {
            root->children[byte]->fail = root;
            queue_push(&q, root->children[byte]);
        }
    }

    while (!queue_is_empty(&q))
    {
        trie_node_t *current = queue_pop(&q);
        for(int byte = 0; byte < ALPHABET_SIZE; byte++)
        {
            trie_node_t *child = current->children[byte];
            if(child == NULL)
                continue;
            trie_node_t *fail_node = current->fail;
            while(fail_node && fail_node->children[byte] == NULL)
            {
                fail_node = fail_node->fail;
            }
            if(fail_node==NULL)
                child->fail = root;
            else
                child->fail = fail_node->children[byte];
            queue_push(&q, child);
        }
    }
}
void trie_free(trie_node_t *node)
{
    if (node == NULL)
        return;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->children[i] != NULL)
        {
            trie_free(node->children[i]);
        }
    }

    free(node);
}
trie_node_t *trie_create(void)
{
    return create_node();  
}