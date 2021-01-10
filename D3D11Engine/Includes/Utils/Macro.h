#pragma once

#ifndef _MACRO_H_
#define _MACRO_H_

// 클래스 기본 생성자와 소멸자를 선언합니다.
#define GEN_CLASS_DEFAULT(CLASS_NAME) \
	CLASS_NAME(); \
	CLASS_NAME(const CLASS_NAME&); \
	~CLASS_NAME(); \

#endif