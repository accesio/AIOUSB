
%define %aioarray_class(TYPE,NAME)
/* #if defined(SWIGPYTHON_BUILTIN) */
/*   %feature("python:slot", "sq_item", functype="ssizeargfunc") NAME::__getitem__; */
/*   %feature("python:slot", "sq_ass_item", functype="ssizeobjargproc") NAME::__setitem__; */


#if defined(SWIGPYTHON)
%exception NAME::__getitem__ {
    if ( arg2 > arg1->_size - 1 || arg2 < 0 ) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return NULL;
    }
    $action
}

%exception NAME::__setitem__ {
    if ( arg2 > arg1->_size - 1 || arg2 < 0 ) {
        PyErr_SetString(PyExc_IndexError,"Index out of range");
        return NULL;
    }
    $action
}
#endif

%inline %{
typedef struct {
    TYPE *el;
    int _size;
} NAME;
%}


#if defined(SWIGJAVA)
%newobject NAME::toString;

#endif


%extend NAME {

  NAME(size_t nelements) {
      NAME *arr = (NAME*)malloc(sizeof(NAME));
      arr->el = (TYPE *)calloc(nelements, sizeof(TYPE));
      arr->_size = (int)nelements;
      return arr;
  }

  ~NAME() {
      free(self->el);
      free(self);
  }
  
#if defined (SWIGRUBY) || defined(SWIGPYTHON)
  TYPE __getitem__(int index) {
      return self->el[index];
  }

  void __setitem__(int index, TYPE value) {
      self->el[index] = value;
  }
#endif
  
#if defined(SWIGPERL) || defined(SWIGJAVA)
  TYPE get(int index) { 
      return self->el[index];
  }

  void set(int index, TYPE value ) {
      self->el[index] = value;
  }
#endif
#if defined(SWIGJAVA)
  #include <stdio.h>
  char *toString() { 
      char *tmp = (char *)calloc(1,sizeof(char));
      tmp[0] = '\0';
      char *typestr;
      if (__builtin_types_compatible_p(__typeof__(self->el[0]), float)) {
          typestr = "%s%s%f";
      } else if (__builtin_types_compatible_p(__typeof__(self->el[0]), double )) {
          typestr = "%s%s%lf";
      } else if (__builtin_types_compatible_p(__typeof__(self->el[0]), long )) {
          typestr = "%s%s%d";
      } else if (__builtin_types_compatible_p(__typeof__(self->el[0]), int )) {
          typestr = "%s%s%d";
      } else if (__builtin_types_compatible_p(__typeof__(self->el[0]), unsigned short )) {
          typestr = "%s%s%u";
      } else {
          typestr = "%s%s%s";
      }
      for ( int i = 0; i < self->_size; i ++ ) {
          char *hold = 0;
          asprintf(&hold,typestr, tmp , ( i == 0 ? "" : "," ), self->el[i] );
          if (tmp) 
              free(tmp);
          tmp = hold;
      }
      return tmp;
  }
#endif

  TYPE * cast() {
      return self->el;
  }

  static NAME *frompointer(TYPE *t) {
      return (NAME *)t;
  }

}


#if !defined(SWIGJAVA)
%extend NAME {
    const char *__repr__() {
        static char buf[BUFSIZ];
        snprintf(buf,BUFSIZ,"TYPE [%d]", self->_size );
        return buf;
    }

    const char *__str__() {
        static char buf[BUFSIZ];
        snprintf(buf,BUFSIZ,"TYPE [%d]", self->_size );
        return buf;
    }
}
#endif
#if defined(SWIGPERL)
%perlcode %{
    {
    package  AIOUSB::NAME;
    sub FETCH {
        my ($self,$key) = @_;
        return $self->get($key);
    }
    sub STORE {
        my($self,$key,$value) = @_;
        $self->set($key,$value);
    }
    }
%}
#endif
%types(NAME = TYPE);


%enddef




