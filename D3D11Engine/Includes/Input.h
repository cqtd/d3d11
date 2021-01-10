#pragma once

#ifndef _INPUT_H_
#define _INPUT_H_

#include "Utils/Macro.h"

class Input
{
public:
	GEN_CLASS_DEFAULT(Input)

public:
	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

#endif