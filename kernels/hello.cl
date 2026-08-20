__kernel void shift_chars(__global char* data) {
    int idx = get_global_id(0);
    data[idx] += 1;
}
