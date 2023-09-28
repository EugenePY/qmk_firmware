#include <stdbool.h>
#include "graphic.h"
#include "qp_stream.h"
#include "qgf.h"
#include "vfat.h"

graphic_node_t* create(graphic_node_t** node, painter_image_handle_t* graphic) {
    *node = malloc(sizeof(graphic_node_t));
    if (*node != NULL) {
        (*node)->value = graphic;
        (*node)->next  = NULL;
        (*node)->last  = NULL;
        return *node;
    } else {
        delete_node(node);
        return *node;
    }
}

void delete_node(graphic_node_t** node) {
    free(*node);
};

void add(graphic_node_t* node, painter_image_handle_t* graphic) {
    while (true) {
        if (node->next != NULL) {
            node = node->next;
        } else {
            create(&node->next, graphic);
            node->next->last = node;
            break;
        }
    }
}

bool forward(graphic_node_t** node) {
    if ((*node)->next != NULL) {
        *node = (*node)->next;
        return true;
    }
    return false;
}
bool backward(graphic_node_t** node) {
    if ((*node)->last != NULL) {
        *node = (*node)->last;
        return true;
    }
    return false;
}

static uint32_t read_graphic_size(uint8_t* buffer) {
    qp_memory_stream_t stream = qp_make_memory_stream(buffer, sizeof(qgf_graphics_descriptor_v1_t));
    return qgf_get_total_size((qp_stream_t*)&stream);
}

void create_nodes(graphic_node_t* root, void* address) {
    uint8_t*        graphic_addr = (uint8_t*)address;
    graphic_node_t* node         = root;
    while (true) {
        painter_image_handle_t next_graphic = qp_load_image_mem(graphic_addr);
        if (next_graphic != NULL) {
            painter_image_handle_t* qp = malloc(sizeof(next_graphic));
            *qp                        = next_graphic;
            add(node, qp);
            node          = node->next;
            uint32_t size = FILE_CLUSTERS(read_graphic_size(graphic_addr)) * SECTOR_SIZE_BYTES * SECTOR_PER_CLUSTER;
            graphic_addr += size;
        } else {
            break;
        }
    }
}
