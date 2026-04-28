#!/bin/bash
# Valgrind suppression file for system libraries
cat > /tmp/wx_suppressions.supp << 'END_OF_SUPPRESSIONS'
# Suppress libAppProtection.so uninitialized memory errors
{
   wx-AppProtection-sendto
   Memcheck:Param
   syscall_socketcall.sendto(msg)
   obj:/usr/local/lib/AppProtection/libAppProtection.so*
}

# Suppress all uninitialized value errors from libAppProtection
{
   wx-AppProtection-uninit
   Memcheck:Cond
   obj:/usr/local/lib/AppProtection/libAppProtection.so*
}

# Suppress librsvg conditional jumps
{
   wx-librsvg-uninit
   Memcheck:Cond
   obj:/usr/lib/x86_64-linux-gnu/librsvg-2.so*
}

# Suppress glib UTF-8 collation invalid reads
{
   wx-glib-invalid-read
   Memcheck:Addr4
   fun:__wcpncpy_avx2
   fun:wcsxfrm_l
   fun:g_utf8_collate_key*
}

{
   wx-glib-invalid-read2
   Memcheck:Addr8
   fun:__wcpncpy_avx2
   fun:wcsxfrm_l
   fun:g_utf8_collate_key*
}

# Suppress all leaks from system libraries (still reachable is normal for GUI apps)
{
   wx-fontconfig-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libfontconfig.so*
}

{
   wx-pango-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libpango-1.0.so*
}

{
   wx-pangoft2-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libpangoft2-1.0.so*
}

{
   wx-gtk3-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libgtk-3.so*
}

{
   wx-gdk-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libgdk-3.so*
}

{
   wx-cairo-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libcairo.so*
}

{
   wx-gobject-leak
   Memcheck:Leak
   match-leak-kinds: all
   fun:g_type_create_instance
   fun:g_object_new*
}

{
   wx-gio-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libgio-2.0.so*
}

{
   wx-expat-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libexpat.so*
}

{
   wx-harfbuzz-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libharfbuzz.so*
}

{
   wx-rsvg-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/librsvg-2.so*
}

{
   wx-gvfs-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/gio/modules/libgvfsdbus.so
}

{
   wx-dconf-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/gio/modules/libdconfsettings.so
}

{
   wx-x11-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libX11.so*
}

{
   wx-xext-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libXext.so*
}

{
   wx-wxwidgets-leak
   Memcheck:Leak
   match-leak-kinds: all
   obj:/usr/lib/x86_64-linux-gnu/libwx_*
}
END_OF_SUPPRESSIONS

LOG_FILE="$(dirname "$0")/valgrind.log"
> "$LOG_FILE"

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --suppressions=/tmp/wx_suppressions.supp \
    --track-origins=yes \
    ./build/dataloader 2>&1 | tee "$LOG_FILE"

echo ""
echo "=== VALGRIND SUMMARY ==="
grep -E "(ERROR SUMMARY|definitely lost|indirectly lost|possibly lost)" "$LOG_FILE" || true
echo ""
echo "Full log written to: $LOG_FILE"
