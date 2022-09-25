struct FontBitmap {
    // TODO: Figure out how to store font asset info and stuff.
};


// NOTE: Reads the whole font file into a char to bitmap lookup table.
// ie: {'A' : FontBitmap, 
//      'B' : FontBitmap}
// etc...
//
// TODO: Should return a bitmap.
FontBitmap* GetFontBitmap(char* font_file_name) {

    return NULL;
}
// NOTE: Displays text at the x and y location
void DisplayText(char* text, int x, int y, int size) {
    // size = 1 means that a 12px bdf font will be 12px.
    // size = 2 means that a 12px bdf font will be 24px. 
    // etc
    
}
