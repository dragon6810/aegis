#pragma once

int loadBytes(const char* filename, char** data);

void LittleEndian(void* data, size_t size);
void BigEndian(void* data, size_t size);