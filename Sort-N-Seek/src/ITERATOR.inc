; ---------------------------------------------------------------------------------------------------------------
; Iterators, a design pattern that let us abstract the container type and 
; design a generic implementation that works on every container. Our 
; sorting and searching implementation is also independent of the container type and 
; accepts iterators of the range and few function pointers. 
;
; ---------------------------------------------------------------------------------------------------------------
; Iterators are nothing but a block of memory. It must contain the following objects:
; 
; 4 bytes: Pointer
; 4 bytes: value_type in bytes (multiples of 4)
; 4 bytes: Function Pointers
; 
; The last object, function pointers, will be the address of an object that holds the following functions
; The following are function pointers. The 'it' in arguments represents 
; the address of an iterator of same type 
; The return values should be moved to the memory location pointed by EDI
; 
; 4 bytes: Next function | returns -> 4-byte address of an iterator | 4x2-byte arguments -> (it, int_offset)
; 4 bytes: Previous function | returns -> 4-byte address of an iterator | 4x2-byte arguments -> (it, int_offset)
; 4 bytes: Copy function | returns -> 4-byte address of an iterator | 4x1-byte argument -> (it)
; 4 bytes: Compare function | returns -> 4-byte int 0, < 0, > 0 in EAX | 4x2-byte arguments -> (it, it)
; 4 bytes: Deref function | returns -> value_type-byte object | 4x1-byte argument -> (it)
; 4 bytes: Distance function | returns -> 4-byte int distance in EAX | 4x2-byte arguments -> (it, it)
; 4 bytes: Swap function | returns -> none | 4x2-byte arguments -> (it, it)
; 
; ---------------------------------------------------------------------------------------------------------------
; Now comes the question, how to initialize an iterator. 
; Answer: Allocate the memory (36 bytes) in stack and move ESP to EDI or some other register. 
; The register now contains the address of iterator-object. Push the address as a function argument. 
;
; ---------------------------------------------------------------------------------------------------------------

Iterator_Functions STRUCT
  _next DD ?
  _prev DD ?
  _copy DD ?
  _comp DD ?
  _dref DD ?
  _dist DD ?
  _swap DD ?
Iterator_Functions ENDS

Iterator STRUCT 
  pointer DD ?
  value_type DD ?
  function_pointers DD ?        ; store the address of an object of Iterator_Functions
Iterator ENDS 
