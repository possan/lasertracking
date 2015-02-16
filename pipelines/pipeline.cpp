#include <vector>
#include <string>
#include <stdlib.h>
#include "pipeline.h"




PipelineEvent::PipelineEvent() {
    type = PE_TYPE_UNKNOWN;
	strcpy(source_node, "?");
	strcpy(source_port, "?");
	strcpy(target_node, "?");
	strcpy(target_port, "?");
	i1 = 0;
	i2 = 0;
	i3 = 0;
	i4 = 0;
	f1 = .0f;
	f2 = .0f;
	f3 = .0f;
	f4 = .0f;
	data = NULL;
	data_length = 0;
	data_free = false;
}

PipelineEvent::PipelineEvent(PipelineEvent *other) {
	type = other->type;
	strcpy(source_node, (other->source_node != NULL) ? other->source_node : "?");
	strcpy(source_port, (other->source_port != NULL) ? other->source_port : "?");
	strcpy(target_node, (other->target_node != NULL) ? other->target_node : "?");
	strcpy(target_port, (other->target_port != NULL) ? other->target_port : "?");
	i1 = other->i1;
	i2 = other->i2;
	i3 = other->i3;
	i4 = other->i4;
	f1 = other->f1;
	f2 = other->f2;
	f3 = other->f3;
	f4 = other->f4;
	if (other->data_free) {
        // we need to actually re-copy the data
        data_free = true;
        data_length = other->data_length;
        data = malloc(other->data_length);
        memcpy(data, other->data, other->data_length);
    } else {
        // just copy the pointers
        data_free = false;
        data = other->data;
        data_length = other->data_length;
    }

}

PipelineEvent::~PipelineEvent() {
	if (data_free && data != NULL) free(data);
}

const char *PipelineEvent::toString() {
	sprintf(m_buf, "[type=%d, data=%lX, %s.%s -> %s.%s, i=%d, f=%f]",
		type, (unsigned long)data, source_node,source_port, target_node,target_port, i1, f1);
	return (const char *)&m_buf;
}





PipelineNode::PipelineNode(const char *id, IPipelineNodeProcessor *processor) {
	this->m_id = strdup(id);
	this->m_processor = processor;
}

PipelineNode::~PipelineNode() {
}

void PipelineNode::handleEvent(PipelineEvent *event) {
	if (this->m_pipeline->m_debug) printf("Pipeline::handleEvent, input=%s\n", event->toString());
	this->m_processor->handleEvent(this, event);
}

void PipelineNode::emit(PipelineEvent *event) {
	if (this->m_pipeline->m_debug) printf("PipelineNode::emit %s\n", event->toString());
	strcpy(event->source_node, m_id);
	this->m_pipeline->queueFrom(this, event);
}





PipelineConnection::PipelineConnection() {
	this->m_source = NULL;
	this->m_output = NULL;
	this->m_target = NULL;
	this->m_input = NULL;
}

PipelineConnection::PipelineConnection(const char *source, const char *output, const char *target, const char *input) {
	this->m_source = strdup(source);
	this->m_output = strdup(output);
	this->m_target = strdup(target);
	this->m_input = strdup(input);
}









Pipeline::Pipeline() {
	m_debug = false;
	m_connections = new std::vector <PipelineConnection*>();
	m_nodes = new std::vector <PipelineNode*>();
	m_eventqueue = new std::queue <PipelineEvent*>();
}

Pipeline::~Pipeline() {
	delete(m_connections);
	delete(m_nodes);
	delete(m_eventqueue);
}

void Pipeline::addNode(PipelineNode *node) {
	m_nodes->push_back(node);
	node->m_pipeline = this; // HACK
}

void Pipeline::addConnection(PipelineConnection *connection) {
	m_connections->push_back(connection);
}

void Pipeline::clearConnections() {
	m_connections->clear();
}

PipelineNode *Pipeline::getNodeById(const char *nodeid) {
	for (std::vector<PipelineNode*>::iterator it = m_nodes->begin() ; it != m_nodes->end(); ++it) {
		PipelineNode *node = *it;
		if (strcmp(node->m_id, nodeid) == 0) {
			return (PipelineNode *)node;
		}
	}
	return NULL;
}

void Pipeline::queueFrom(PipelineNode *node, PipelineEvent *event) {
	for (std::vector<PipelineConnection*>::iterator it = m_connections->begin() ; it != m_connections->end(); ++it) {
		PipelineConnection *c = *it;
		if (strcmp(event->source_node, c->m_source) == 0 &&
			strcmp(event->source_port, c->m_output) == 0) {
			PipelineEvent *eventcopy = new PipelineEvent(event);
			strcpy(eventcopy->target_node, c->m_target);
			strcpy(eventcopy->target_port, c->m_input);
			if (this->m_debug) printf("Pipeline::queueFrom, event=%s\n", eventcopy->toString());
			m_eventqueue->push(eventcopy);
		}
	}
}

void Pipeline::queue(PipelineEvent *event) {
	if (this->m_debug) printf("Pipeline::queueFrom, event=%s\n", event->toString());
	m_eventqueue->push(event);
}

void Pipeline::broadcast(PipelineEvent *event) {
	// send to all nodes, in order.
	for (std::vector<PipelineNode*>::iterator it = m_nodes->begin() ; it != m_nodes->end(); ++it) {
		PipelineNode *node = *it;
		node->handleEvent(event);
	}
}

int Pipeline::queueSize() {
    return m_eventqueue->size();
}

bool Pipeline::flushQueue(int max, int iter) {
	// if (this->m_debug) printf("Pipeline::flushQueue\n");
	if (m_eventqueue->empty())
		return false;

	bool any_flushed = false;
	for(int j=0; j<iter; j++) {
		for(int i=0; i<max; i++) {
            // printf("queue size: %ud\n", m_eventqueue->size());
            if (m_eventqueue->empty())
                continue;

            PipelineEvent *event = m_eventqueue->front();
            
            if (event == NULL) {
				break;
			}

            m_eventqueue->pop();

			if (this->m_debug) printf("flushQueue; event=%s\n", event->toString());

			PipelineNode *target_node = getNodeById(event->target_node);
			if (target_node != NULL) target_node->handleEvent(event);
			// free(event);
		}
	}

	return any_flushed;
}

