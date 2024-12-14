/* -------------------------------------------------------------------------- */
#ifdef SAMPLE_SMOOTHING
void initQueue(struct StationQueue *q, int size) {
	q->size  = size;
	q->sum   = 0;
	q->nodes = (struct TempSampleNode*) malloc(size * sizeof(struct TempSampleNode));
	q->first = &q->nodes[0];
	q->last  = &q->nodes[size - 1];

	int i;

	for (i = 0; i < size; ++i) {
		q->nodes[i].val = 0;
	}

	for (i = 0; i < size - 1; ++i) {
		q->nodes[i].next = &q->nodes[i + 1];
	}

	for (i = size - 1; i > 0; --i) {
		q->nodes[i].prev = &q->nodes[i - 1];
	}

	q->first->prev = q->last;
	q->last->next  = q->first;

}
#endif
/* -------------------------------------------------------------------------- */
#ifdef SAMPLE_SMOOTHING
void addQueueNode(struct StationQueue *q, int newNodeVal) {
	// remove the value of first node from sum:
	q->sum -= q->last->val;

	// shft pointers:
	q->first = q->first->prev;
	q->last  = q->last->prev;

	// add new value to queue (and to the overall sum):
	q->sum       += newNodeVal;
	q->first->val = newNodeVal;
}
#endif
/* -------------------------------------------------------------------------- */
