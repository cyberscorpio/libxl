#include <iostream>
#include <sstream>
#include <utility>
#include "../libxl/include/dp/Observable.h"


static std::stringstream ss;

class CObj : public xl::dp::CObserableT<CObj> {
public:
	enum EVENT : xl::dp::CObserableT<CObj>::EVT {
		EVT_1,
		EVT_2,
		EVT_NUM
	};
public:
	// ...
	void doEvt1() {
		_TriggerEvent (EVT_1, NULL);
	}
	void doEvt2() {
		_TriggerEvent (EVT_2, "param");
	}
};

class CObserver1 : public CObj::IObserver {
public:
	virtual void onEvent (EVT evt, void *param) {
		char buf[1024];
		if (param == NULL) {
			param = "NULL";
		}
		sprintf(buf, "1 get %d %s", (int)evt + 1, param);
		ss << buf;
	}
};

class CObserver2 : public CObj::IObserver {
public:
	virtual void onEvent (EVT evt, void *param) {
		char buf[1024];
		if (param == NULL) {
			param = "NULL";
		}
		sprintf(buf, "2 get %d %s", (int)evt + 1, param);
		ss << buf;
	}
};


#ifdef IN_IDE
int test_observable(int argc, char **argv) {
#else
int main(int argc, char **argv) {
#endif
	CObj  obj;
	CObserver1 observer1;
	CObserver2 observer2;

	obj.subscribe(&observer1);
	obj.doEvt2();
	const char *case1 = "1 get 2 param";
	if (ss.str() == case1) {
		std::cout << "case1 pass" << std::endl;
	} else {
		std::cout << "case1 failed =>" << ss.str() <<  std::endl;
	}
	ss.str("");
	
	obj.subscribe(&observer2, false);
	obj.doEvt1();
	obj.doEvt2();
	const char *case2 = "2 get 1 NULL1 get 1 NULL2 get 2 param1 get 2 param";
	if (ss.str() == case2) {
		std::cout << "case2 pass" << std::endl;
	} else {
		std::cout << "case2 failed =>" << ss.str() << std::endl;
	}
	ss.str("");


	obj.unsubscribe(&observer1);
	obj.doEvt1();
	const char *case3 = "2 get 1 NULL";
	if (ss.str() == case3) {
		std::cout << "case3 pass" << std::endl;
	} else {
		std::cout << "case3 failed" << std::endl;
	}
	ss.str("");

	obj.unsubscribe(&observer2);
	obj.doEvt2();
	const char *case4 = "";
	if (ss.str() == case4) {
		std::cout << "case4 pass" << std::endl;
	} else {
		std::cout << "case4 failed" << std::endl;
	}
	ss.str("");
	
	return 0;
}