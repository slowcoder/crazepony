#pragma once

#include "error.h"

err_code motors_init(void);
void     motors_set_output(int fr,int rr,int rl,int fl);
