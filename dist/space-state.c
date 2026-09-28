// Read-only snapshot for live validation. No event tap or event posting.
// clang -I Sources/ISS/include dist/space-state.c Sources/ISS/*.c \
//   -framework ApplicationServices -framework CoreFoundation -o build/space-state
#include "ISS.h"
#include <stdio.h>

int main(void) {
    ISSSpaceInfo cursor = {0}, menu = {0};
    bool cursorOK = iss_get_space_info(&cursor);
    bool menuOK = iss_get_menubar_space_info(&menu);
    printf("cursor: ok=%d display=%s index=%u count=%u\n", cursorOK,
           cursor.displayID, cursor.currentIndex + 1, cursor.spaceCount);
    printf("menu: ok=%d display=%s index=%u count=%u\n", menuOK,
           menu.displayID, menu.currentIndex + 1, menu.spaceCount);
    return cursorOK && menuOK ? 0 : 1;
}
