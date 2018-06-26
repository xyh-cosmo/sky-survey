#include "SurveySim.h"

void initCMGNode( struct CMG_Node* node,
                  double st,
                  double cv,
                  double ut,
                  struct CMG_Node* next ) {
    node->s_time=st;
    node->cmg_value = cv;
    node->use_time = ut;
    node->next = next;
}

void initCMGList(struct CMG_List* list) {
    list->head = NULL;
    list->tail = NULL;
    list->cmg_total = 0;
    list->time_dura = 0;
}

void addCMG_Node( struct CMG_List* list,
                  struct CMG_Node* node ) {
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
        list->cmg_total = list->cmg_total + node->cmg_value;
        list->time_dura = node->use_time;
    } else {
        list->tail->next = node;
        list->tail = list->tail->next;
        list->tail->next = NULL;
        list->cmg_total = list->cmg_total + node->cmg_value;
        double t1 = list->head->s_time;
        double t2 = list->tail->s_time;
        list->time_dura = t2-t1+list->tail->use_time;
    }
}


struct CMG_Node* PopCMGNode(struct CMG_List* list) {
    struct CMG_Node* rNode = list->head;
    if(list->head->next != NULL) {
        double t1 = list->head->s_time;
        double cmg_v = list->head->cmg_value;

        list->head = list->head->next;

        double t2 = list->head->s_time;
        list->time_dura = list->time_dura - (t2-t1);
        list->cmg_total = list->cmg_total - cmg_v;
    } else {
        list->head = NULL;
        list->tail = NULL;
        list->time_dura = 0;
        list->cmg_total = 0;
    }
    return rNode;
}

void freeCMG_Node(struct CMG_Node* node) {
    node->next = NULL;
    free(node);
}

void freeCMG_List(struct CMG_List* list) {
    while(list->head != NULL) {
        struct CMG_Node* node = list->head;
        list->head = list->head->next;
        freeCMG_Node(node);
    }
    free(list);
}

double get_cmg_use(double tAngle){

    double cmg_use=0.0;
    
    // double tm_k;
    // if( tAngle <= 5.0 ) {
    //     cmg_use = 0;
    // } else if( tAngle <= 10.0 ) {
    //     tm_k = (1/29.0-0)/(10.0-5.0);
    //     cmg_use = tm_k*(tAngle-5);
    // } else if( tAngle <= 20.0 ) {
    //     tm_k = (1/19.0-1/29.0)/(20.0-10.0);
    //     cmg_use = tm_k*(tAngle-10)+1/29.0;
    // } else if( tAngle <= 35.0 ) {
    //     tm_k = (1/13.0-1/19.0)/(35.0-20.0);
    //     cmg_use = tm_k*(tAngle-20)+1/19.0;
    // } else if( tAngle <= 45.0 ) {
    //     tm_k = (1/10.0-1/13.0)/(45.0-35.0);
    //     cmg_use = tm_k*(tAngle-35)+1/13.0;
    // } else if( tAngle <= 75.0 ) {
    //     tm_k = (1/6.0-1/10.0)/(75.0-45.0);
    //     cmg_use = tm_k*(tAngle-45)+1/10.0;
    // } else if( tAngle <= 90.0 ) {
    //     tm_k = (1/5.0-1/6.0)/(90.0-75.0);
    //     cmg_use = tm_k*(tAngle-75)+1/6.0;
    // } else if( tAngle <= 135.0 ) {
    //     tm_k = (1/3.0-1/5.0)/(135.0-90.0);
    //     cmg_use = tm_k*(tAngle-90)+1/5.0;
    // } else if( tAngle <= 180.0 ) {
    //     tm_k = (1/2.0-1/3.0)/(180.0-135.0);
    //     cmg_use = tm_k*(tAngle-135)+1/3.0;
    // }
    
    if( tAngle < 5.0 ) {
        cmg_use = 0;
    } else if( tAngle >=  5.0 && tAngle < 10.0 ) {
        cmg_use = 1./29.;
    } else if( tAngle >= 10.0 && tAngle < 20.0 ) {
        cmg_use = 1./19.;
    } else if( tAngle >= 20.0 && tAngle < 35.0 ) {
        cmg_use = 1./13.;
    } else if( tAngle >= 35.0 && tAngle < 45.0 ) {
        cmg_use = 1./10.;
    } else if( tAngle >= 45.0 && tAngle < 75.0 ) {
        cmg_use = 1./6.;
    } else if( tAngle >= 75.0 && tAngle < 90.0 ) {
        cmg_use = 1./5.;
    } else if( tAngle >= 90.0 && tAngle < 135.0 ) {
        cmg_use = 1./3.;
    } else if( tAngle >= 135.0 && tAngle <= 180.0 ) {
        cmg_use = 1./2.;
    }

    return cmg_use;
}

// int main(int argc, char* argv[]){

// 	struct CMG_List * list = NULL;
// 	list = (struct CMG_List*)malloc(sizeof(struct CMG_List));
// 	initCMGList(list);

// 	if (list ==NULL){
// 		printf("Error !!\n");
// 	}
// 	int i = 0;
// 	for(i = 0;i < 100; i ++){

// 		struct CMG_Node* node = NULL;
// 		node = (struct CMG_Node*) malloc(sizeof(struct CMG_Node));
// 		initCMGNode(node,i+10,0.01,2.0,NULL);
// 		addCMG_Node(list,node);

// 	}

// 	struct CMG_Node* i_node = list->head;
// 	while(i_node !=NULL){
// 		printf("%f    %f\n",list->cmg_total,list->time_dura);
// 		i_node = i_node->next;
// 	}


// 	freeCMG_List(list);

// 	return 1;
// }
