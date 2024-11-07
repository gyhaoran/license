#ifndef D1CDC975_672C_4CEE_A838_CF554ED53F72
#define D1CDC975_672C_4CEE_A838_CF554ED53F72

#define DISALLOW_COPY_AND_ASSIGN(ClassName)     \
DISALLOW_ASSIGN(ClassName) 						\
DISALLOW_COPY(ClassName)

#define DISALLOW_ASSIGN(ClassName)             \
private:                                       \
    ClassName& operator=(const ClassName&);

#define DISALLOW_COPY(ClassName)               \
private:                                       \
    ClassName(const ClassName&);               \

#endif /* D1CDC975_672C_4CEE_A838_CF554ED53F72 */
