#include "pipeline.h"
#include <stdlib.h>
#include <stdio.h>



#define PT_USER_STRING (PE_TYPE_USER + 1)

//
// Generator node - Generates an incrementing number
//
// Output example:
//   "output" type=PT_USER_STRING data="inputstring.inputstring"
//

class GenNode : public IPipelineNodeProcessor {
	private:
		int counter;
		char m_buffer[500];
	public:
		GenNode();
		void handleEvent(PipelineNode *node, PipelineEvent *event);
};

GenNode::GenNode() {
	counter = 0;
}

void GenNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
	// printf("GenNode(#%s):handleEvent, event=%s\n", node->m_id, event->toString());
	if (event->type == PE_TYPE_TICK) {
		counter ++;
		PipelineEvent e2;
		e2.type = PT_USER_STRING;
		e2.i1 = counter;
		sprintf(m_buffer, "%d.%d", counter, rand()%10);
		e2.data = &m_buffer;
		strcpy(e2.source_port, "output");
		node->emit(&e2);
	}
}


//
// Modifier node - Duplicates input strings
//
// Input example:
//   "input" type=PT_USER_STRING data="inputstring"
//
// Output example:
//   "output" type=PT_USER_STRING data="inputstring.inputstring"
//

class ModNode : public IPipelineNodeProcessor {
	private:
		char m_buffer[1500];
	public:
		ModNode();
		void handleEvent(PipelineNode *node, PipelineEvent *event);
};

ModNode::ModNode() { }

void ModNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
	// printf("ModNode(#%s):handleEvent, event=%s\n", node->m_id, event->toString());
	if (strcmp(event->target_port, "input") == 0 && event->type == PT_USER_STRING) {
		printf("Modifying string from %s.%s: %s\n", event->source_node, event->source_port, event->data);
		PipelineEvent e2;
		e2.type = PT_USER_STRING;
		sprintf(m_buffer, "(%s %s)#%d", event->data, event->data, rand()%10);
		e2.data = &m_buffer;
		strcpy(e2.source_port, "output");
		printf("Output string: %s\n", e2.data);
		node->emit(&e2);
	}
}


//
// Sink node - Prints incoming packet strings
//
// Input example:
//   "input" type=PT_USER_STRING data="inputstring"
//

class SinkNode : public IPipelineNodeProcessor {
	public:
		SinkNode();
		void handleEvent(PipelineNode *node, PipelineEvent *event);
};

SinkNode::SinkNode() { }

void SinkNode::handleEvent(PipelineNode *node, PipelineEvent *event) {
	// printf("SinkNode(#%s):handleEvent, event=%s\n", node->m_id, event->toString());
	if (strcmp(event->target_port, "input") == 0 && event->type == PT_USER_STRING) {
		printf("Sink got string from %s.%s: %s\n", event->source_node, event->source_port, event->data);
	}
}

//
// Test code
//

void test1() {
	printf("* test1\n");
	printf("  * setup\n");
	Pipeline *p = new Pipeline();
	p->m_debug = true;
	p->addNode(new PipelineNode("gen", new GenNode()));
	p->addNode(new PipelineNode("p1", new ModNode()));
	p->addNode(new PipelineNode("p2", new ModNode()));
	p->addNode(new PipelineNode("p2-2", new ModNode()));
	p->addNode(new PipelineNode("p2-3", new ModNode()));
	p->addNode(new PipelineNode("p3", new ModNode()));
	p->addNode(new PipelineNode("p3-2", new ModNode()));
	p->addNode(new PipelineNode("sink", new SinkNode()));
    p->clearConnections();
	p->addConnection(new PipelineConnection("gen", "output", "p1", "input"));
	p->addConnection(new PipelineConnection("gen", "output", "p2", "input"));
	p->addConnection(new PipelineConnection("gen", "output", "p3", "input"));
	p->addConnection(new PipelineConnection("p1", "output", "sink", "input"));
	p->addConnection(new PipelineConnection("p2", "output", "p2-2", "input"));
	p->addConnection(new PipelineConnection("p2-2", "output", "p2-3", "input"));
	p->addConnection(new PipelineConnection("p2-3", "output", "sink", "input"));
	p->addConnection(new PipelineConnection("p3", "output", "p3-2", "input"));
	p->addConnection(new PipelineConnection("p3-2", "output", "sink", "input"));
    printf("\n");
    for(int l=0; l<1000; l++) {
        printf("  * tick\n");
        PipelineEvent e1;
        e1.type = PE_TYPE_TICK;
        p->broadcast(&e1);
        p->flushQueue(30, 2);
        printf("\n");
    }
	printf("  * done\n");
	delete(p);
}

int main() {
	test1();
}
