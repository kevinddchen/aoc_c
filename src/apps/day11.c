#include "vector.h"

#include <assert.h>
#include <stdio.h>

static const char FILENAME[] = "files/day11.txt";

static const long DAY11_PART1_ANS = 613;

// Number of 3-alpha ids.
static const size_t NUM_NODE_IDS = 3 * 26;

// Special node ids
static const char YOU[] = "you";
static const char OUT[] = "out";

// Sentinel value for unset `Node.num_paths`.
static const int UNSET_NUM_PATHS = -1;

typedef struct {
    // Vector of `Node*`
    Vector parents;
    // Vector of `Node*`
    Vector children;
    // Number of paths arriving at this node from the node "you"
    int num_paths;
} Node;

/**
 * Converts 3-character id, like "aaa", to a numerical index.
 */
size_t id_to_index(const char* id)
{
    return 26 * 26 * (id[0] - 'a') + 26 * (id[1] - 'a') + (id[2] - 'a');
}

/**
 * Given node id, like "aaa", return pointer to the `Node` in the graph.
 */
Node* id_to_node(const char* id, Node** graph)
{
    const size_t index = id_to_index(id);
    Node* ptr = graph[index];
    assert(ptr != NULL);
    return ptr;
}

/**
 * Return the number of paths from "you" to the node. This equals the sum of `recurse_num_paths()` over the parents, so
 * this function determines the answer recursively. We cache the result on the node.
 */
int recurse_num_paths(Node* node)
{
    if (node->num_paths != UNSET_NUM_PATHS)
        return node->num_paths;  // cache hit

    node->num_paths = 0;
    for (size_t i = 0; i < node->parents.count; i++) {
        Node* parent = *(Node**)vector_at(&node->parents, i);
        node->num_paths += recurse_num_paths(parent);
    }
    return node->num_paths;
}

/**
 * Read graph from file.
 * @param filename Path to text file.
 * @param nodes Output array of `Node`. Ownership will be transferred to the function caller.
 * @param graph Output graph. Ownership will be transferred to the function caller.
 * @returns Number of nodes in the graph.
 */
size_t read_graph(const char* filename, Node** nodes_ptr, Node*** graph_ptr)
{
    FILE* fp = fopen(filename, "r");
    assert(fp != NULL);

    // 1. Get number of nodes
    size_t num_nodes = 1;  // "out" is a node potentially not listed in the file
    int c = {};
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n')
            num_nodes++;
    }

    // 2. `nodes` is an array that owns all the graph data
    Node* nodes = malloc(num_nodes * sizeof(Node));
    assert(nodes != NULL);
    for (size_t i = 0; i < num_nodes; i++) {
        vector_init(&nodes[i].parents, sizeof(Node*));
        vector_init(&nodes[i].children, sizeof(Node*));
        nodes[i].num_paths = UNSET_NUM_PATHS;
    }

    // 3. Graph is implemented as an id -> `Node*` lookup table
    Node** graph = malloc(NUM_NODE_IDS * sizeof(Node*));
    assert(graph != NULL);
    for (size_t i = 0; i < NUM_NODE_IDS; i++)
        graph[i] = NULL;

    fseek(fp, 0, SEEK_SET);  // set file pointer to beginning of file
    char buff[1024] = {};
    size_t line_num = 0;
    while (fgets(buff, sizeof buff, fp) != NULL) {
        const size_t index = id_to_index(buff);
        graph[index] = nodes + line_num;
        line_num++;
    }
    // add "out" as node, if not already added
    const size_t out_index = id_to_index(OUT);
    if (graph[out_index] == NULL)
        graph[out_index] = nodes + line_num;

    // 4. Populate node parents and children
    fseek(fp, 0, SEEK_SET);  // set file pointer to beginning of file
    while (fgets(buff, sizeof buff, fp) != NULL) {
        Node* parent_node = id_to_node(buff, graph);
        char* ptr = buff;
        while (*ptr != ':')
            ptr++;
        ptr += 2;  // advance past ': ' to start of first id

        while (*ptr != '\0') {
            Node* child_node = id_to_node(ptr, graph);
            vector_push_back(&parent_node->children, &child_node);
            vector_push_back(&child_node->parents, &parent_node);
            ptr += 4;
        }
    }

    fclose(fp);
    fp = NULL;

    *nodes_ptr = nodes;
    *graph_ptr = graph;
    return num_nodes;
}

void free_nodes(Node* nodes, size_t num_nodes)
{
    for (size_t i = 0; i < num_nodes; i++) {
        vector_free(&nodes[i].parents);
        vector_free(&nodes[i].children);
    }
}

int main()
{
    Node* nodes;
    Node** graph;
    const size_t num_nodes = read_graph(FILENAME, &nodes, &graph);

    // Base case for num_paths
    Node* you_node = id_to_node(YOU, graph);
    you_node->num_paths = 1;

    // Compute num_paths for all nodes recursively
    Node* out_node = id_to_node(OUT, graph);
    recurse_num_paths(out_node);

    printf("Day 11\n");
    printf("Part 1: %d\n", out_node->num_paths);

    assert(out_node->num_paths == DAY11_PART1_ANS);

    free_nodes(nodes, num_nodes);
    free(graph);

    return EXIT_SUCCESS;
}
