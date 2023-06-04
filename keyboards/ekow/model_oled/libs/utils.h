#ifdef SEMIHOST_ENABLE
#    define SEMIHOSTING_SYS_WRITE0 0x04
static inline int32_t semihosting_call(int32_t R0, int32_t R1) {
    int32_t rc;
    __asm__ volatile("mov r0, %1\n" /* move int R0 to register r0 */
                     "mov r1, %2\n" /* move int R1 to register r1 */
                     "bkpt #0xAB\n" /* thumb mode semihosting call */
                     "mov %0, r0"   /* move register r0 to int rc */
                     : "=r"(rc)
                     : "r"(R0), "r"(R1)
                     : "r0", "r1", "ip", "lr", "memory", "cc");
    return rc;
}

static void semihosting_write_string(char *string) {
    semihosting_call(SEMIHOSTING_SYS_WRITE0, (uintptr_t)string);
}

initialise_monitor_handles();

#endif
