#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <vector>
#include <queue>

class Pipeline;
class PipelineNode;

//
// Events
//

#define PE_TYPE_UNKNOWN 0
#define PE_TYPE_TICK 1
#define PE_TYPE_USER 100

#define NODENAMESIZE 32
#define PORTSIZE 8
#define TOSTRINGSIZE 100

class PipelineEvent {
    private:
        char m_buf[TOSTRINGSIZE];
	public:
		unsigned short type;
		char source_node[NODENAMESIZE];
        char source_port[PORTSIZE];
        char target_node[NODENAMESIZE];
        char target_port[PORTSIZE];
		int i1;
		int i2;
		int i3;
		int i4;
		float f1;
		float f2;
		float f3;
		float f4;
		void *data;
		long data_length;
		bool data_free;
		PipelineEvent();
		PipelineEvent(PipelineEvent *other);
		~PipelineEvent();
		const char *toString();
};

//
// Interface for event processors
//

class IPipelineNodeProcessor {
	public:
		virtual ~IPipelineNodeProcessor() {};
		virtual void handleEvent(PipelineNode *node, PipelineEvent *event) = 0;
};

//
// Wrapper around each node in the graph
//

class PipelineNode {
	public:
		const char *m_id;
		IPipelineNodeProcessor *m_processor;
		Pipeline *m_pipeline;

	public:
		PipelineNode(const char *id, IPipelineNodeProcessor *processor);
		~PipelineNode();

		// Sends incoming event to node processor
		void handleEvent(PipelineEvent *event);

		// Call this function from within your node processor handler
		void emit(PipelineEvent *event);
};

//
// Connection information
//

class PipelineConnection {
	public:
		const char *m_source;
		const char *m_target;
		const char *m_input;
		const char *m_output;
		PipelineConnection();
		PipelineConnection(const char *source, const char *output, const char *target, const char *input);
		~PipelineConnection();
};

//
// Forward iterating graph
//

class Pipeline {
	private:
		std::vector <PipelineConnection*> *m_connections;
		std::vector <PipelineNode*> *m_nodes;
		std::queue <PipelineEvent*> *m_eventqueue;

	public:
		bool m_debug;

	public:
		Pipeline();
		~Pipeline();

		// Adds a node to the graph
		void addNode(PipelineNode *node);

		// Connects two nodes in the graph
		void addConnection(PipelineConnection *connection);

		// Disconnect everything
		void clearConnections();

		// Gets a node by it's id
		PipelineNode *getNodeById(const char *nodeid);

		// Queues outgoing event FROM a node to all it's receivers
		void queueFrom(PipelineNode *node, PipelineEvent *event);

		// Queues incoming event
		void queue(PipelineEvent *event);
    
        // Queue size
        int queueSize();

		// Tries to flush nodequeue, returns true if anything flushed.
		bool flushQueue(int max=50, int iter=15);

		// Broadcast event to all nodes
		void broadcast(PipelineEvent *event);
};

#endif