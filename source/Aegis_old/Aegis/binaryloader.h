#pragma once

int loadBytes(const char* filename, char** data);

void LittleEndian(void* data, long size);
void BigEndian(void* data, long size);
