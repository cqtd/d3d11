#pragma once

#ifndef _MACRO_H_
#define _MACRO_H_

// Ŭ���� �⺻ �����ڿ� �Ҹ��ڸ� �����մϴ�.
#define GEN_CLASS_DEFAULT(CLASS_NAME) \
	CLASS_NAME(); \
	CLASS_NAME(const CLASS_NAME&); \
	~CLASS_NAME(); \

#endif