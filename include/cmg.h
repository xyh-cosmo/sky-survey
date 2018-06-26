#ifndef _CMG_H_
#define _CMG_H_

#define CMG_THRES 1.0
#define ORBIT_TIME 0.0638

struct CMG_Node{
	double s_time;		// 队列起始时间
	double use_time; 	// exposure time + trans time
	double cmg_value;	// 权重。。。
	struct CMG_Node *next;
};

struct CMG_List {
	struct CMG_Node* head;
	struct CMG_Node* tail;
	double cmg_total;	// 总的权重
	double time_dura;	// 队列的总时间
};

// #ifndef CMG_LIST
//    #define CMG_LIST
   struct CMG_List* cmg_list;
// #endif

struct CMG_Node* PopCMGNode(struct CMG_List*);
void initCMGNode(struct CMG_Node*,double, double, double, struct CMG_Node*);
void initCMGList(struct CMG_List*);
void addCMG_Node(struct CMG_List*,struct CMG_Node*);
void freeCMG_Node(struct CMG_Node*);
void freeCMG_List(struct CMG_List*);


double get_cmg_use(double tAngle);

#endif