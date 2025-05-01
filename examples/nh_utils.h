#include <stdarg.h> // For va_list, va_start, va_arg, va_end
#include <stddef.h> // For size_t
#include <stdint.h> // For uint32_t etc.

namespace nh
{
// --- Minimal snprintf Implementation ---

// Helper function to reverse a string segment [start, end]
static void reverse_string_segment(char *start, char *end) {
    char tmp;
    while (start < end) {
        tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
}

// Helper function: Basic long unsigned int to ASCII conversion
// Writes number into 'buf', null-terminated. Returns number of digits written.
// Assumes buf has sufficient size (e.g., 21 chars for 64-bit unsigned long + null).
static int simple_lutoa(long unsigned int value, char *buf) {
    char *p = buf;

    // Handle 0 explicitly
    if (value == 0) {
        *p++ = '0';
        *p = '\0';
        return 1;
    }

    // Convert digits in reverse order
    while (value > 0) {
        *p++ = '0' + (value % 10);
        value /= 10;
    }

    // Null terminate and reverse
    *p = '\0';
    int len = p - buf;
    reverse_string_segment(buf, p - 1);
    return len;
}

// Simplified snprintf implementation
// Supports: %% %lu %s %d %u (basic signed/unsigned int)
// Returns: Number of characters written (excluding null), or -1 on encoding error (not implemented here)
// IMPORTANT: Does NOT return the number of chars that *would* have been written like standard snprintf.
int snprintf(char *str, size_t size, const char *format, ...) {
    if (str == NULL || size == 0) {
        return 0; // Nothing can be written
    }

    va_list args;
    va_start(args, format);

    char *p = str;
    size_t remaining = size - 1; // Leave space for the null terminator
    int written_count = 0;

    while (*format != '\0' && remaining > 0) {
        if (*format != '%') {
            // Normal character
            *p++ = *format++;
            remaining--;
            written_count++;
        } else {
            // Format specifier
            format++; // Skip '%'

            switch (*format) {
                case '\0': // Dangling '%' at end of format string
                    *p++ = '%'; // Output the '%'
                    remaining--;
                    written_count++;
                    goto end_format_loop; // Stop processing

                case '%': // Literal '%%'
                    *p++ = '%';
                    remaining--;
                    written_count++;
                    format++; // Skip the second '%'
                    break;

                case 'l': // Check for 'lu'
                    format++; // Skip 'l'
                    if (*format == 'u') {
                        long unsigned int val = va_arg(args, long unsigned int);
                        char num_buf[21]; // Max 20 digits for 64-bit ulong + null
                        int num_len = simple_lutoa(val, num_buf);
                        char *num_p = num_buf;
                        while (num_len-- > 0 && remaining > 0) {
                            *p++ = *num_p++;
                            remaining--;
                            written_count++;
                        }
                        format++; // Skip 'u'
                    } else {
                        // Unsupported format like "%ld", treat as literal "%l" + char
                        *p++ = '%'; written_count++; remaining--;
                        if (remaining > 0) {
                            *p++ = 'l'; written_count++; remaining--;
                        }
                        // The character after 'l' (which wasn't 'u') will be handled in the next loop iteration
                    }
                    break;

                 case 'u': // Unsigned int (assuming int size)
                 case 'd': // Signed int (assuming int size)
                 case 'i': // Signed int (assuming int size)
                 {
                     int is_signed = (*format != 'u');
                     int val_signed = 0;
                     unsigned int val_unsigned = 0;
                     char num_buf[12]; // Max 11 digits for 32-bit int + sign + null
                     char *num_p = num_buf;
                     int num_len = 0;

                     if(is_signed) {
                         val_signed = va_arg(args, int);
                         if (val_signed < 0) {
                              if(remaining > 0) { // Check space for sign
                                   *p++ = '-';
                                   remaining--;
                                   written_count++;
                              }
                              // Convert to positive for lutoa (beware INT_MIN edge case if implementing fully)
                              val_unsigned = (unsigned int)-val_signed;
                         } else {
                              val_unsigned = (unsigned int)val_signed;
                         }
                         num_len = simple_lutoa(val_unsigned, num_buf); // Use lutoa, handles 0
                     } else {
                         val_unsigned = va_arg(args, unsigned int);
                         // Use lutoa directly, handles 0
                         num_len = simple_lutoa(val_unsigned, num_buf);
                     }

                     while (num_len-- > 0 && remaining > 0) {
                         *p++ = *num_p++;
                         remaining--;
                         written_count++;
                     }
                     format++; // Skip 'u', 'd', or 'i'
                     break;
                 }


                case 's': // String
                    {
                        const char *s_arg = va_arg(args, const char *);
                        if (s_arg == NULL) {
                            s_arg = "(null)";
                        }
                        while (*s_arg != '\0' && remaining > 0) {
                            *p++ = *s_arg++;
                            remaining--;
                            written_count++;
                        }
                        format++; // Skip 's'
                    }
                    break;

                default: // Unknown specifier, print literally
                    *p++ = '%';
                    remaining--;
                    written_count++;
                    if (remaining > 0) {
                        *p++ = *format; // Print the unknown char
                        remaining--;
                        written_count++;
                    }
                    format++; // Skip unknown char
                    break;
            }
        }
    }

end_format_loop:
    *p = '\0'; // Always null-terminate
    va_end(args);
    return written_count;
}

// --- End of Minimal snprintf Implementation ---


// --- Basic fmodf Implementation ---

// Simple truncf implementation if standard one is missing.
// WARNING: Limited range (relies on long long conversion), undefined for NaN/Inf.
/*
static inline float truncf(float x) {
    // Basic version using cast, may not handle edge cases perfectly
    return (float)((long long)x);
}
*/

/**
 * Basic implementation of fmodf(x, y).
 * Calculates the floating-point remainder of x / y.
 * The result has the same sign as x and magnitude less than y.
 *
 * @param x Numerator (float)
 * @param y Denominator (float)
 * @return The remainder. Returns 0 if y is 0. Does not handle NaN/Inf robustly.
 */
static inline float fmodf(float x, float y) {
    // Handle y == 0 case (return 0 as a simple fallback)
    if (y == 0.0f) {
        return 0.0f;
        // Alternative: return NAN; // if NAN is defined and needed
    }

    // Use standard library truncf if available (preferred)
    float n = truncf(x / y);

    // --- FALLBACK if truncf is undefined ---
    // If you get an "undefined symbol: truncf" error:
    // 1. Uncomment the my_truncf function above this one.
    // 2. Comment out the line above: // float n = truncf(x / y);
    // 3. Uncomment the line below:
    // float n = my_truncf(x / y);
    // --- End Fallback ---

    // Calculate remainder: r = x - n * y
    float remainder = x - n * y;

    return remainder;
}

// Aligns ptr UP to the next multiple of 'alignment' (alignment must be power of 2)
static inline uint8_t* align_pointer(uint8_t* ptr, size_t alignment) {
    uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t mask = alignment - 1;
    p = (p + mask) & ~mask;
    return reinterpret_cast<uint8_t*>(p);
}

}
