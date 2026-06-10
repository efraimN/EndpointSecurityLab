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

#ifndef CASTMACRO
#define CASTMACRO
inline LONGLONG UPCAST(DWORD x)
{
	LARGE_INTEGER tmpCasting = { 0 };
	tmpCasting.LowPart = x;
	return tmpCasting.QuadPart;
 }
inline LONGLONG UPCASTHANDLE(DWORD x)
{
#ifdef _M_X64
	LARGE_INTEGER tmpCasting = { 0 };
	tmpCasting.LowPart = x;
	return tmpCasting.QuadPart;
#else
	return (DWORD)x;
#endif
}
inline DWORD DOWNCAST(LONGLONG x)
{
	LARGE_INTEGER tmpCasting = { 0 };
	tmpCasting.QuadPart = x;
	return tmpCasting.LowPart;
}
inline DWORD DOWNCASTHANDLE(PVOID x)
{
#ifdef _M_X64
	LARGE_INTEGER tmpCasting = { 0 };
	tmpCasting.QuadPart = (LONGLONG)x;
	return tmpCasting.LowPart;
#else
	return (DWORD)x;
#endif
}
inline DWORD DOWNCASTULONGPTR(ULONG_PTR x)
{
	return DOWNCASTHANDLE((PVOID)x);
}
#endif
