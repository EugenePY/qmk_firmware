#pragma once

#include "qp_comms.h"

// simple link list
typedef struct graphic_node_s graphic_node_t;
struct graphic_node_s {
    painter_image_handle_t* value;
    graphic_node_t*         last;
    graphic_node_t*         next;
};

typedef struct graphic_list_s {
    graphic_node_t* root;
} graphic_list_t;

// create node
graphic_node_t* create(graphic_node_t** node, painter_image_handle_t* graphic);
void            delete_node(graphic_node_t** node); // delete the node
void            add(graphic_node_t* node, painter_image_handle_t* graphic);
bool            forward(graphic_node_t** node);
bool            backward(graphic_node_t** node);
void            create_nodes(graphic_node_t* root, void* address);
