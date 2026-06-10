/*
Copyright (c) 2026 - present, Ephraim Neuberger. All rights reserved.

The code and materials provided by Ephraim Neuberger are for non-commercial testing and evaluation purposes only.
Ephraim Neuberger reserves all rights not expressly granted.

Any permitted copy must retain this copyright notice and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL EPHRAIM NEUBERGER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define FILE__NAME_x FILE__NAME
#define INCLUDE_FILE(TAIL) TO_STR( CAT_2(FILE__NAME_x,TAIL) )

#pragma message ( TO_STR(INT_DIR) "\\" INCLUDE_FILE(.tmh) "(0) : WPP MESSAGE: File Included")

#pragma warning(push)
#pragma warning(disable:26440 26493 26494 26477 26485 26489 4003)
#include INCLUDE_FILE(.tmh)
#pragma warning(pop)

#undef INCLUDE_FILE





