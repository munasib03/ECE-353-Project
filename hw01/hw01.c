/**
 * @file hw01.c
 * @author
 * @brief
 * @version 0.1
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "hw01.h"

/**
 * @brief
 * Prints out the grid of the game board
 * @param print_debug
 * If set to true, print out additional debug information about the location of the borders to
 * the serial console
 */
void memory_draw_grid(bool print_debug)
{
    /* Clear the game board area */
    lcd_draw_rectangle_centered(
        LOC_X_GAME_BOARD_CENTER,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BOARD,
        HEIGHT_BOARD,
        LCD_COLOR_BLACK);

    if (print_debug)
    {
        /* Print out the location of each of the horizontal lines */
        printf("Time is located at (%d,%d)\n\r", LOC_X_CENTER, LOC_Y_TIME);
        printf("Row0 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_X_CENTER, LOC_Y_BORDER_ROW0, WIDTH_BOARD, HEIGHT_BORDER);
        printf("Row1 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_X_CENTER, LOC_Y_BORDER_ROW1, WIDTH_BOARD, HEIGHT_BORDER);
        printf("Row2 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_X_CENTER, LOC_Y_BORDER_ROW2, WIDTH_BOARD, HEIGHT_BORDER);
        printf("Row3 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_X_CENTER, LOC_Y_BORDER_ROW3, WIDTH_BOARD, HEIGHT_BORDER);
        printf("Row4 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_X_CENTER, LOC_Y_BORDER_ROW4, WIDTH_BOARD, HEIGHT_BORDER);
    }

    lcd_draw_rectangle_centered(
        LOC_X_CENTER,
        LOC_Y_BORDER_ROW0,
        WIDTH_BOARD,
        HEIGHT_BORDER,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_CENTER,
        LOC_Y_BORDER_ROW1,
        WIDTH_BOARD,
        HEIGHT_BORDER,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_CENTER,
        LOC_Y_BORDER_ROW2,
        WIDTH_BOARD,
        HEIGHT_BORDER,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_CENTER,
        LOC_Y_BORDER_ROW3,
        WIDTH_BOARD,
        HEIGHT_BORDER,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_CENTER,
        LOC_Y_BORDER_ROW4,
        WIDTH_BOARD,
        HEIGHT_BORDER,
        LCD_COLOR_ORANGE);

    /* Draw the horizontal Lines on the screen */

    if (print_debug)
    {
        /* Print out the location of each of the vertical lines */
        printf("Col0 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_Y_BORDER_ROW0, LOC_Y_CENTER, WIDTH_BORDER, HEIGHT_BOARD);
        printf("Col1 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_Y_BORDER_ROW1, LOC_Y_CENTER, WIDTH_BORDER, HEIGHT_BOARD);
        printf("Col2 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_Y_BORDER_ROW2, LOC_Y_CENTER, WIDTH_BORDER, HEIGHT_BOARD);
        printf("Col3 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_Y_BORDER_ROW3, LOC_Y_CENTER, WIDTH_BORDER, HEIGHT_BOARD);
        printf("Col4 is located at (%d,%d) with dimensions of (%d,%d) pixels\n\r", LOC_Y_BORDER_ROW4, LOC_Y_CENTER, WIDTH_BORDER, HEIGHT_BOARD);
    }

    /* Draw the vertical lines on the screen */
    lcd_draw_rectangle_centered(
        LOC_X_BORDER_COL0,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BORDER,
        HEIGHT_BOARD,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_BORDER_COL1,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BORDER,
        HEIGHT_BOARD,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_BORDER_COL2,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BORDER,
        HEIGHT_BOARD,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_BORDER_COL3,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BORDER,
        HEIGHT_BOARD,
        LCD_COLOR_ORANGE);

    lcd_draw_rectangle_centered(
        LOC_X_BORDER_COL4,
        LOC_Y_GAME_BOARD_CENTER,
        WIDTH_BORDER,
        HEIGHT_BOARD,
        LCD_COLOR_ORANGE);
}

/**
 * @brief
 *  Prints out a single decimal number at the specified coordinates.
 * @param num
 *      Number to Print
 * @param x
 *      X coordinate
 * @param y
 *      Y coordinate
 * @param fcolor
 *      Foreground color
 * @param bcolor
 *      Background color
 */
void memory_print_number(uint8_t num, uint16_t x, uint16_t y, uint16_t fcolor, uint16_t bcolor)
{
    uint32_t bitmap_addr = 0;
    uint16_t width = 0;
    uint16_t height = 0;

    /* Verify num is a number from 0-9 OR the ASCII character ':' */
    if (((num >= 0) && (num <= 9)) || (num == 58))
    {
        /* If num is a ':', then we need to change the index into the LUT to be 10*/
        if (num == 58)
        {
            num = 10;
        }

        /* Get the height of the bitmap from the Font Look Up Table */
        height = Shentox_Desc[num].h_px;

        /* Get the width of the bitmap from the Font Look Up Table */
        width = Shentox_Desc[num].w_px;

        /* Get the address of the bitmap from the Font Look Up Table */
        bitmap_addr = (uint32_t)(&IMAGE_NUMBERS[Shentox_Desc[num].index]);

        lcd_draw_image(x, y, width, height, (const uint8_t *)bitmap_addr, fcolor, bcolor);
    }
}

/**
 * @brief
 *  Draws the specified image at the coordinates specified by row and col
 * @param row
 *      Specifies which row the image is located in
 * @param col
 *      Specifies which column the image is located in
 * @param image
 *      Specifies which image to draw
 * @param fcolor
 *      Specifies the foreground color of the image
 * @param bcolor
 *      Specifies the background color of the image
 */
void memory_draw_image(uint8_t row, uint8_t col, mem_image_t image, uint16_t fcolor, uint16_t bcolor)
{
    // Variables that hold the properties of the shapes to be passed into the lcd_draw_image() function.

    uint16 width = 0;  // Width in pixels of shape
    uint16 height = 0; // Height in pixels of shape
    uint8_t *picture;  // A pointer to the array that holds the shape

    // Based on the row number, assign row to the correct position macro
    // Range: 0<=row<=3;
    if (row == 0)
    {
        row = LOC_Y_IMAGE_ROW0;
    }
    else if (row == 1)
    {
        row = LOC_Y_IMAGE_ROW1;
    }
    else if (row == 2)
    {
        row = LOC_Y_IMAGE_ROW2;
    }
    else if (row == 3)
    {
        row = LOC_Y_IMAGE_ROW3;
    }
    else if (row == 5)         //row = 5 refers to center of the board vertically
    {
        row = LOC_Y_CENTER;
    }
    else
    { // error message for debugging, row is out of range.
        printf("Row Undefined, printing nothing.");
        return;
    }

    // Repeat procedure with column,
    // Range: 0<=col<=3
    if (col == 0)
    {
        col = LOC_X_IMAGE_COL0;
    }
    else if (col == 1)
    {
        col = LOC_X_IMAGE_COL1;
    }
    else if (col == 2)
    {
        col = LOC_X_IMAGE_COL2;
    }
    else if (col == 3)
    {
        col = LOC_X_IMAGE_COL3;
    }else if (col == 5)         //Col = 5 refers to center of the board horizontally
    {
        col = LOC_X_CENTER;
    }
    else
    { // error message for debugging, col out of range.
        printf("Column Undefined, printing nothing.");
        return;
    }

    // Compare the enum to find the correlating shape to the number.
    // Once correct shape is selected, update the shape dimensions (Height + Width).

    if(image == MEM_IMAGE_CLUB){
       picture = IMAGE_CLUB;
       width = IMAGE_CLUB_PIXEL_WIDTH;
       height = IMAGE_CLUB_PIXEL_HEIGHT;
    }else if (image == MEM_IMAGE_SPADE){
       picture = IMAGE_SPADE;
       width = IMAGE_SPADE_PIXEL_WIDTH;
       height = IMAGE_SPADE_PIXEL_HEIGHT;
    }else if (image == MEM_IMAGE_HEART){
       picture = IMAGE_HEART;
       width = IMAGE_HEART_PIXEL_WIDTH;
       height = IMAGE_HEART_PIXEL_HEIGHT;
    }else if (image == MEM_IMAGE_DIAMOND){
       picture = IMAGE_DIAMOND;
       width = IMAGE_DIAMOND_PIXEL_WIDTH;
       height = IMAGE_DIAMOND_PIXEL_HEIGHT;
    }else if (image == MEM_IMAGE_QUESTION){
       picture = IMAGE_QUESTION;
       width = IMAGE_QUESTION_PIXEL_WIDTH;
       height = IMAGE_QUESTION_PIXEL_HEIGHT;
    }else if (image == 5){
       picture = IMAGE_GAME_OVER;
       width = IMAGE_GAME_OVER_PIXEL_WIDTH;
       height = IMAGE_GAME_OVER_PIXEL_HEIGHT;
    }
    // After selecting the correct image, draw image on the LCD.
    lcd_draw_image(col, row, width, height, picture, fcolor, bcolor);
}

/**
 * @brief
 * Erases an image at the specified coordinates by drawing a black rectangle
 * @param row
 * @param col
 */
void memory_erase_image(uint8_t row, uint8_t col)
{
    // Compare the row number (0-3) to select the correct grid to be erased.
    if (row == 0)
    {
        row = LOC_Y_IMAGE_ROW0;
    }
    else if (row == 1)
    {
        row = LOC_Y_IMAGE_ROW1;
    }
    else if (row == 2)
    {
        row = LOC_Y_IMAGE_ROW2;
    }
    else if (row == 3)
    {
        row = LOC_Y_IMAGE_ROW3;
    }
    else if (row == 5)         //row = 5 refers to center of the board vertically
    {
        row = LOC_Y_CENTER;
    }
    else
    { // Error message for debugging, Row out of scope.
        printf("Row Undefined, printing nothing.");
        return;
    }
    // Compare col to range (0-3) to select the correct grid column to be erased.
    if (col == 0)
    {
        col = LOC_X_IMAGE_COL0;
    }
    else if (col == 1)
    {
        col = LOC_X_IMAGE_COL1;
    }
    else if (col == 2)
    {
        col = LOC_X_IMAGE_COL2;
    }
    else if (col == 3)
    {
        col = LOC_X_IMAGE_COL3;
    }
    else if (col == 5)         //Col = 5 refers to center of the board horizontally
    {
        col = LOC_X_CENTER;
    }
    else
    { // Error message for debugging (Column out of scope).
        printf("Column Undefined, printing nothing.");
        return;
    }
    // Draw a black rectangle over the grid to "Erase" using the dimension macros.
    lcd_draw_rectangle_centered(col, row, WIDTH_IMAGE, HEIGHT_IMAGE, LCD_COLOR_BLACK);
}
