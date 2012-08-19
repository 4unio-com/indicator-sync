/* -- THIS FILE IS GENERATED - DO NOT EDIT *//* -*- Mode: C; c-basic-offset: 4 -*- */

#include <Python.h>



#line 29 "syncindicator.override"
#include <Python.h>
#include <indicator-sync/sync-enum.h>
#include <indicator-sync/sync-client.h>
#include <glib.h>
#include "pygobject.h"
#include "pyglib.h"

typedef PyObject* (*to_pyobject_func) (gpointer data);

typedef struct
{
		PyGObject       *listener;
		PyObject        *callback;
        to_pyobject_func pyobject_convert;
		PyObject        *user_data;        
} ListenerPropertyCbData;

static void
_listener_get_property_cb(IndicateListener * listener, 
						  IndicateListenerServer * server, 
						  IndicateListenerIndicator * indicator, 
						  gchar * property, gpointer propertydata, 
						  ListenerPropertyCbData *data)
{
        PyGILState_STATE state;
        PyObject *args, *rv, *pyserver, *pyindicator, *pypropertydata = NULL;

        state = pyg_gil_state_ensure();

        pypropertydata = data->pyobject_convert(propertydata);

        pyserver = pyg_pointer_new (INDICATE_TYPE_LISTENER_SERVER, server);
        pyindicator = pyg_pointer_new (INDICATE_TYPE_LISTENER_INDICATOR, indicator);

        if (data->user_data == Py_None)
        {
                args = Py_BuildValue("OOOsO", data->listener, 
                                     pyserver, 
                                     pyindicator,
                                     property,
                                     pypropertydata);
        }
        else
        {
                args = Py_BuildValue("OOOsOO", data->listener, 
                                     pyserver, 
                                     pyindicator,
                                     property,
                                     pypropertydata,
                                     data->user_data);
        }

        Py_DECREF(data->user_data);

        rv = PyEval_CallObject(data->callback, args);

        if (rv == NULL)
                PyErr_Print();
        else
                Py_DECREF(rv);

        Py_DECREF(args);
        Py_DECREF(data->callback);
        g_free(data);

        pyg_gil_state_release(state);
}

static void
_listener_get_server_property_cb(IndicateListener * listener, 
                                 IndicateListenerServer * server, 
                                 gchar *value,
                                 ListenerPropertyCbData *data)
{
		PyGILState_STATE state;
		PyObject *args, *rv, *pyserver;

        pyserver = pyg_pointer_new (INDICATE_TYPE_LISTENER_SERVER, server);

		state = pyg_gil_state_ensure();

		if (data->user_data == Py_None)
		{
				args = Py_BuildValue("OOs", data->listener, 
									 pyserver, 
									 value);
		}
		else
		{
				args = Py_BuildValue("OOsO", data->listener, 
									 pyserver, 
									 value,
									 data->user_data);
		}

		Py_DECREF(data->user_data);

		rv = PyEval_CallObject(data->callback, args);

		if (rv == NULL)
				PyErr_Print();
		else
				Py_DECREF(rv);

		Py_DECREF(args);
		Py_DECREF(data->callback);
		g_free(data);

		pyg_gil_state_release(state);
}

void
_indicate_add_constants(PyObject *module, const gchar *strip_prefix)
{
#ifdef VERSION
        PyModule_AddStringConstant(module, "__version__", VERSION);
#endif
        pyg_enum_add(module, "Interests", 
                     strip_prefix, INDICATE_TYPE_INTERESTS);
        
        if (PyErr_Occurred())
                PyErr_Print();
}
#line 132 "syncindicator.c"


/* ---------- types from other modules ---------- */
static PyTypeObject *_PyGObject_Type;
#define PyGObject_Type (*_PyGObject_Type)
static PyTypeObject *_PyGdkPixbuf_Type;
#define PyGdkPixbuf_Type (*_PyGdkPixbuf_Type)


/* ---------- forward type declarations ---------- */
PyTypeObject G_GNUC_INTERNAL PyIndicateIndicator_Type;
PyTypeObject G_GNUC_INTERNAL PyIndicateListener_Type;
PyTypeObject G_GNUC_INTERNAL PyIndicateServer_Type;

#line 147 "syncindicator.c"



/* ----------- IndicateListenerServer ----------- */

static int
pygobject_no_constructor(PyObject *self, PyObject *args, PyObject *kwargs)
{
    gchar buf[512];

    g_snprintf(buf, sizeof(buf), "%s is an abstract widget", self->ob_type->tp_name);
    PyErr_SetString(PyExc_NotImplementedError, buf);
    return -1;
}

static PyObject *
_wrap_indicate_listener_server_get_dbusname(PyObject *self)
{
    const gchar *ret;

    ret = indicate_listener_server_get_dbusname(pyg_pointer_get(self, IndicateListenerServer));
    if (ret)
        return PyString_FromString(ret);
    Py_INCREF(Py_None);
    return Py_None;
}

static const PyMethodDef _PyIndicateListenerServer_methods[] = {
    { "get_dbusname", (PyCFunction)_wrap_indicate_listener_server_get_dbusname, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyIndicateListenerServer_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "indicate.ListenerServer",                   /* tp_name */
    sizeof(PyGPointer),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    0,             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyIndicateListenerServer_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    0,                 /* tp_dictoffset */
    (initproc)pygobject_no_constructor,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- IndicateListenerIndicator ----------- */

static PyObject *
_wrap_indicate_listener_indicator_get_id(PyObject *self)
{
    guint ret;

    ret = indicate_listener_indicator_get_id(pyg_pointer_get(self, IndicateListenerIndicator));
    return PyLong_FromUnsignedLong(ret);
}

static const PyMethodDef _PyIndicateListenerIndicator_methods[] = {
    { "get_id", (PyCFunction)_wrap_indicate_listener_indicator_get_id, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyIndicateListenerIndicator_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "indicate.ListenerIndicator",                   /* tp_name */
    sizeof(PyGPointer),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    0,             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyIndicateListenerIndicator_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    0,                 /* tp_dictoffset */
    (initproc)pygobject_no_constructor,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- IndicateIndicator ----------- */

static int
_wrap_indicate_indicator_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char* kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     ":indicate.Indicator.__init__",
                                     kwlist))
        return -1;

    pygobject_constructv(self, 0, NULL);
    if (!self->obj) {
        PyErr_SetString(
            PyExc_RuntimeError, 
            "could not create indicate.Indicator object");
        return -1;
    }
    return 0;
}

static PyObject *
_wrap_indicate_indicator_show(PyGObject *self)
{
    
    indicate_indicator_show(INDICATE_INDICATOR(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_hide(PyGObject *self)
{
    
    indicate_indicator_hide(INDICATE_INDICATOR(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_is_visible(PyGObject *self)
{
    int ret;

    
    ret = indicate_indicator_is_visible(INDICATE_INDICATOR(self->obj));
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_indicate_indicator_get_id(PyGObject *self)
{
    guint ret;

    
    ret = indicate_indicator_get_id(INDICATE_INDICATOR(self->obj));
    
    return PyLong_FromUnsignedLong(ret);
}

static PyObject *
_wrap_indicate_indicator_user_display(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "timestamp", NULL };
    PyObject *py_timestamp = NULL;
    guint timestamp = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O:Indicate.Indicator.user_display", kwlist, &py_timestamp))
        return NULL;
    if (py_timestamp) {
        if (PyLong_Check(py_timestamp))
            timestamp = PyLong_AsUnsignedLong(py_timestamp);
        else if (PyInt_Check(py_timestamp))
            timestamp = PyInt_AsLong(py_timestamp);
        else
            PyErr_SetString(PyExc_TypeError, "Parameter 'timestamp' must be an int or a long");
        if (PyErr_Occurred())
            return NULL;
    }
    
    indicate_indicator_user_display(INDICATE_INDICATOR(self->obj), timestamp);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_set_property(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "key", "data", NULL };
    char *key, *data;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"ss:Indicate.Indicator.set_property", kwlist, &key, &data))
        return NULL;
    
    indicate_indicator_set_property(INDICATE_INDICATOR(self->obj), key, data);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_gtk_indicator_set_property_icon(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "key", "data", NULL };
    char *key;
    PyGObject *data;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"sO!:Indicate.Indicator.set_property_icon", kwlist, &key, &PyGdkPixbuf_Type, &data))
        return NULL;
    
    indicate_gtk_indicator_set_property_icon(INDICATE_INDICATOR(self->obj), key, GDK_PIXBUF(data->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

#line 169 "syncindicator.override"
static PyObject *
_wrap_indicate_indicator_set_property_time(PyGObject *self, PyObject *args)
{
		double time;
		gchar *key;
		GTimeVal g_time, *g_timep;

		if (!PyArg_ParseTuple(args, "sd:Indicator.set_property_time", 
							  &key, &time))
		{
				return NULL;
		}

		if (time > 0.0) {
				g_time.tv_sec = (glong) time;
				g_time.tv_usec = (glong)((time - g_time.tv_sec)
										 * G_USEC_PER_SEC);
				g_timep = &g_time;
		} else if (time == 0.0) {
				g_timep = NULL;
		} else {
				PyErr_SetString(PyExc_ValueError, "time must be >= 0.0");
				return NULL;
		}

		indicate_indicator_set_property_time(INDICATE_INDICATOR(self->obj), 
											 key, g_timep);

		Py_INCREF(Py_None);
		return Py_None;
}
#line 446 "syncindicator.c"


static PyObject *
_wrap_indicate_indicator_set_property_int(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "key", "value", NULL };
    char *key;
    int value;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"si:Indicate.Indicator.set_property_int", kwlist, &key, &value))
        return NULL;
    
    indicate_indicator_set_property_int(INDICATE_INDICATOR(self->obj), key, value);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_set_property_bool(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "key", "value", NULL };
    char *key;
    int value;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"si:Indicate.Indicator.set_property_bool", kwlist, &key, &value))
        return NULL;
    
    indicate_indicator_set_property_bool(INDICATE_INDICATOR(self->obj), key, value);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_get_property(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "key", NULL };
    char *key;
    const gchar *ret;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"s:Indicate.Indicator.get_property", kwlist, &key))
        return NULL;
    
    ret = indicate_indicator_get_property(INDICATE_INDICATOR(self->obj), key);
    
    if (ret)
        return PyString_FromString(ret);
    Py_INCREF(Py_None);
    return Py_None;
}

#line 523 "syncindicator.override"
static PyObject *
_wrap_indicate_indicator_list_properties(PyGObject *self)
{		
		GPtrArray *property_list;
		PyObject *pyprop_list;
		guint i;

		property_list = \
				indicate_indicator_list_properties(
						INDICATE_INDICATOR(self->obj));

		pyprop_list = PyList_New (property_list->len);

		for (i=0; i < property_list->len; i++) {
				PyList_SET_ITEM(
						pyprop_list, i, 
						PyString_FromString(
								g_ptr_array_index(property_list, i)));
                g_free(g_ptr_array_index(property_list, i));
        }

        g_ptr_array_free (property_list, TRUE);

		return pyprop_list;
}
#line 525 "syncindicator.c"


static PyObject *
_wrap_indicate_indicator_set_displayed(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "displayed", NULL };
    int displayed;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"i:Indicate.Indicator.set_displayed", kwlist, &displayed))
        return NULL;
    
    indicate_indicator_set_displayed(INDICATE_INDICATOR(self->obj), displayed);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_indicator_get_displayed(PyGObject *self)
{
    int ret;

    
    ret = indicate_indicator_get_displayed(INDICATE_INDICATOR(self->obj));
    
    return PyBool_FromLong(ret);

}

static const PyMethodDef _PyIndicateIndicator_methods[] = {
    { "show", (PyCFunction)_wrap_indicate_indicator_show, METH_NOARGS,
      NULL },
    { "hide", (PyCFunction)_wrap_indicate_indicator_hide, METH_NOARGS,
      NULL },
    { "is_visible", (PyCFunction)_wrap_indicate_indicator_is_visible, METH_NOARGS,
      NULL },
    { "get_id", (PyCFunction)_wrap_indicate_indicator_get_id, METH_NOARGS,
      NULL },
    { "user_display", (PyCFunction)_wrap_indicate_indicator_user_display, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_property", (PyCFunction)_wrap_indicate_indicator_set_property, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_property_icon", (PyCFunction)_wrap_indicate_gtk_indicator_set_property_icon, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_property_time", (PyCFunction)_wrap_indicate_indicator_set_property_time, METH_VARARGS,
      NULL },
    { "set_property_int", (PyCFunction)_wrap_indicate_indicator_set_property_int, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_property_bool", (PyCFunction)_wrap_indicate_indicator_set_property_bool, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_property", (PyCFunction)_wrap_indicate_indicator_get_property, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "list_properties", (PyCFunction)_wrap_indicate_indicator_list_properties, METH_VARARGS,
      NULL },
    { "set_displayed", (PyCFunction)_wrap_indicate_indicator_set_displayed, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_displayed", (PyCFunction)_wrap_indicate_indicator_get_displayed, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyIndicateIndicator_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "indicate.Indicator",                   /* tp_name */
    sizeof(PyGObject),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyIndicateIndicator_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_indicate_indicator_new,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- IndicateListener ----------- */

static int
_wrap_indicate_listener_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char* kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     ":indicate.Listener.__init__",
                                     kwlist))
        return -1;

    pygobject_constructv(self, 0, NULL);
    if (!self->obj) {
        PyErr_SetString(
            PyExc_RuntimeError, 
            "could not create indicate.Listener object");
        return -1;
    }
    return 0;
}

static PyObject *
_wrap_indicate_listener_display(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "server", "indicator", "timestamp", NULL };
    PyObject *py_server, *py_indicator, *py_timestamp = NULL;
    IndicateListenerIndicator *indicator = NULL;
    guint timestamp = 0;
    IndicateListenerServer *server = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"OOO:Indicate.Listener.display", kwlist, &py_server, &py_indicator, &py_timestamp))
        return NULL;
    if (pyg_pointer_check(py_server, INDICATE_TYPE_LISTENER_SERVER))
        server = pyg_pointer_get(py_server, IndicateListenerServer);
    else {
        PyErr_SetString(PyExc_TypeError, "server should be a IndicateListenerServer");
        return NULL;
    }
    if (pyg_pointer_check(py_indicator, INDICATE_TYPE_LISTENER_INDICATOR))
        indicator = pyg_pointer_get(py_indicator, IndicateListenerIndicator);
    else {
        PyErr_SetString(PyExc_TypeError, "indicator should be a IndicateListenerIndicator");
        return NULL;
    }
    if (py_timestamp) {
        if (PyLong_Check(py_timestamp))
            timestamp = PyLong_AsUnsignedLong(py_timestamp);
        else if (PyInt_Check(py_timestamp))
            timestamp = PyInt_AsLong(py_timestamp);
        else
            PyErr_SetString(PyExc_TypeError, "Parameter 'timestamp' must be an int or a long");
        if (PyErr_Occurred())
            return NULL;
    }
    
    indicate_listener_display(INDICATE_LISTENER(self->obj), server, indicator, timestamp);
    
    Py_INCREF(Py_None);
    return Py_None;
}

#line 466 "syncindicator.override"
static PyObject *
_wrap_indicate_listener_server_get_type(PyGObject *self, 
                                        PyObject *args,
                                        PyObject *kwargs)
{
		static char *kwlist[] = { "server", "callback", "user_data", NULL };
		ListenerPropertyCbData *listener_property_cb_data;
		PyGObject *server;
		PyGILState_STATE state;
		PyObject *callback, *user_data = Py_None;
		size_t len;

		state = pyg_gil_state_ensure();

		len = PyTuple_Size(args);

		if (len < 2)
		{
				PyErr_SetString(PyExc_TypeError,
								"IndicateListener.server_get_type requires at least "
								"3 arguments");
				return NULL;
		}

		if (!PyArg_ParseTupleAndKeywords(args, kwargs,
										 "OO|O:IndicateListener.server_get_type",
										 kwlist,
										 &server, &callback, &user_data))
		{
				return NULL;
		}

		if (!PyCallable_Check(callback))
		{
				PyErr_SetString(PyExc_TypeError, "second argument must be callable");
				return NULL;
		}

		listener_property_cb_data = g_new0(ListenerPropertyCbData, 1);
		listener_property_cb_data->callback = callback;
		listener_property_cb_data->user_data = user_data;
		listener_property_cb_data->listener = self;

		Py_INCREF(callback);
		Py_INCREF(user_data);

		indicate_listener_server_get_type(INDICATE_LISTENER(self->obj),
										  (IndicateListenerServer *)server->obj, 
										  (indicate_listener_get_server_property_cb)_listener_get_server_property_cb,
										  listener_property_cb_data);

		Py_INCREF(Py_None);
		pyg_gil_state_release(state);
		return Py_None;
}
#line 752 "syncindicator.c"


#line 408 "syncindicator.override"
static PyObject *
_wrap_indicate_listener_server_get_desktop(PyGObject *self, 
										   PyObject *args,
										   PyObject *kwargs)
{
		static char *kwlist[] = { "server", "callback", "user_data", NULL };
		ListenerPropertyCbData *listener_property_cb_data;
		PyGObject *server;
		PyGILState_STATE state;
		PyObject *callback, *user_data = Py_None;
		size_t len;

		state = pyg_gil_state_ensure();

		len = PyTuple_Size(args);

		if (len < 2)
		{
				PyErr_SetString(PyExc_TypeError,
								"IndicateListener.get_property requires at least "
								"3 arguments");
				return NULL;
		}

		if (!PyArg_ParseTupleAndKeywords(args, kwargs,
										 "OO|O:IndicateListener.get_property",
										 kwlist,
										 &server, &callback, &user_data))
		{
				return NULL;
		}

		if (!PyCallable_Check(callback))
		{
				PyErr_SetString(PyExc_TypeError, "second argument must be callable");
				return NULL;
		}

		listener_property_cb_data = g_new0(ListenerPropertyCbData, 1);
		listener_property_cb_data->callback = callback;
		listener_property_cb_data->user_data = user_data;
		listener_property_cb_data->listener = self;

		Py_INCREF(callback);
		Py_INCREF(user_data);

		indicate_listener_server_get_desktop(
                INDICATE_LISTENER(self->obj),
                (IndicateListenerServer *)server->obj, 
                (indicate_listener_get_server_property_cb)_listener_get_server_property_cb,
                listener_property_cb_data);

		Py_INCREF(Py_None);
		pyg_gil_state_release(state);
		return Py_None;
}
#line 812 "syncindicator.c"


static PyObject *
_wrap_indicate_listener_server_show_interest(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "server", "interest", NULL };
    PyObject *py_server, *py_interest = NULL;
    IndicateListenerServer *server = NULL;
    IndicateInterests interest;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"OO:Indicate.Listener.server_show_interest", kwlist, &py_server, &py_interest))
        return NULL;
    if (pyg_pointer_check(py_server, INDICATE_TYPE_LISTENER_SERVER))
        server = pyg_pointer_get(py_server, IndicateListenerServer);
    else {
        PyErr_SetString(PyExc_TypeError, "server should be a IndicateListenerServer");
        return NULL;
    }
    if (pyg_enum_get_value(INDICATE_TYPE_INTERESTS, py_interest, (gpointer)&interest))
        return NULL;
    
    indicate_listener_server_show_interest(INDICATE_LISTENER(self->obj), server, interest);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_listener_server_remove_interest(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "server", "interest", NULL };
    PyObject *py_server, *py_interest = NULL;
    IndicateListenerServer *server = NULL;
    IndicateInterests interest;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"OO:Indicate.Listener.server_remove_interest", kwlist, &py_server, &py_interest))
        return NULL;
    if (pyg_pointer_check(py_server, INDICATE_TYPE_LISTENER_SERVER))
        server = pyg_pointer_get(py_server, IndicateListenerServer);
    else {
        PyErr_SetString(PyExc_TypeError, "server should be a IndicateListenerServer");
        return NULL;
    }
    if (pyg_enum_get_value(INDICATE_TYPE_INTERESTS, py_interest, (gpointer)&interest))
        return NULL;
    
    indicate_listener_server_remove_interest(INDICATE_LISTENER(self->obj), server, interest);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_listener_server_check_interest(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "server", "interest", NULL };
    PyObject *py_server, *py_interest = NULL;
    int ret;
    IndicateListenerServer *server = NULL;
    IndicateInterests interest;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"OO:Indicate.Listener.server_check_interest", kwlist, &py_server, &py_interest))
        return NULL;
    if (pyg_pointer_check(py_server, INDICATE_TYPE_LISTENER_SERVER))
        server = pyg_pointer_get(py_server, IndicateListenerServer);
    else {
        PyErr_SetString(PyExc_TypeError, "server should be a IndicateListenerServer");
        return NULL;
    }
    if (pyg_enum_get_value(INDICATE_TYPE_INTERESTS, py_interest, (gpointer)&interest))
        return NULL;
    
    ret = indicate_listener_server_check_interest(INDICATE_LISTENER(self->obj), server, interest);
    
    return PyBool_FromLong(ret);

}

#line 202 "syncindicator.override"
static PyObject *
_convert_string_to_pyobject(gpointer data)
{
        return PyString_FromString((gchar *)data);
}

static PyObject *
_wrap_indicate_listener_get_indicator_property(PyGObject *self, 
											   PyObject *args,
											   PyObject *kwargs)
{
		static char *kwlist[] = { "server", "indicator", "property", "callback",
								  "user_data", NULL };
		ListenerPropertyCbData *listener_property_cb_data;
		gchar *property;
        PyGObject *server, *indicator;
		PyGILState_STATE state;
		PyObject *callback, *user_data = Py_None;
		size_t len;

		state = pyg_gil_state_ensure();

		len = PyTuple_Size(args);

		if (len < 4)
		{
				PyErr_SetString(PyExc_TypeError,
								"IndicateListener.get_property requires at least "
								"3 arguments");
				return NULL;
		}

		if (!PyArg_ParseTupleAndKeywords(args, kwargs,
										 "OOsO|O:IndicateListener.get_property",
										 kwlist,
										 &server, &indicator, &property, &callback, &user_data))
		{
				return NULL;
		}

		if (!PyCallable_Check(callback))
		{
				PyErr_SetString(PyExc_TypeError, "fourth argument must be callable");
				return NULL;
		}

		listener_property_cb_data = g_new0(ListenerPropertyCbData, 1);
		listener_property_cb_data->callback = callback;
		listener_property_cb_data->user_data = user_data;
		listener_property_cb_data->listener = self;
		listener_property_cb_data->pyobject_convert = _convert_string_to_pyobject;

		Py_INCREF(callback);
		Py_INCREF(user_data);

		indicate_listener_get_property(INDICATE_LISTENER(self->obj),
									   (IndicateListenerServer *)server->obj, 
									   (IndicateListenerIndicator *)indicator->obj,
									   property,
									   (indicate_listener_get_property_cb)_listener_get_property_cb,
									   listener_property_cb_data);

		Py_INCREF(Py_None);
		pyg_gil_state_release(state);
		return Py_None;
}
#line 958 "syncindicator.c"


#line 270 "syncindicator.override"
static PyObject *
_convert_pixbuf_to_pyobject(gpointer data)
{
        return pygobject_new(data);
}

static PyObject *
_wrap_indicate_listener_get_indicator_property_icon(PyGObject *self, 
                                                    PyObject *args,
                                                    PyObject *kwargs)
{
		static char *kwlist[] = { "server", "indicator", "property", "callback",
								  "user_data", NULL };
		ListenerPropertyCbData *listener_property_cb_data;
		gchar *property;
        PyGObject *server, *indicator;
		PyGILState_STATE state;
		PyObject *callback, *user_data = Py_None;
		size_t len;

		state = pyg_gil_state_ensure();

		len = PyTuple_Size(args);

		if (len < 4)
		{
				PyErr_SetString(PyExc_TypeError,
								"IndicateListener.get_property requires at least "
								"3 arguments");
				return NULL;
		}

		if (!PyArg_ParseTupleAndKeywords(args, kwargs,
										 "OOsO|O:IndicateListener.get_property",
										 kwlist,
										 &server, &indicator, &property, &callback, &user_data))
		{
				return NULL;
		}

		if (!PyCallable_Check(callback))
		{
				PyErr_SetString(PyExc_TypeError, "fourth argument must be callable");
				return NULL;
		}

		listener_property_cb_data = g_new0(ListenerPropertyCbData, 1);
		listener_property_cb_data->callback = callback;
		listener_property_cb_data->user_data = user_data;
		listener_property_cb_data->listener = self;
		listener_property_cb_data->pyobject_convert = _convert_pixbuf_to_pyobject;

		Py_INCREF(callback);
		Py_INCREF(user_data);

		indicate_gtk_listener_get_property_icon(
				INDICATE_LISTENER(self->obj),
				(IndicateListenerServer *)server->obj, 
				(IndicateListenerIndicator *)indicator->obj,
				property,
				(indicate_gtk_listener_get_property_icon_cb)_listener_get_property_cb,
				listener_property_cb_data);

		Py_INCREF(Py_None);
		pyg_gil_state_release(state);
		return Py_None;
}
#line 1029 "syncindicator.c"


#line 339 "syncindicator.override"
PyObject *
_convert_time_to_pyobject(gpointer data)
{
        GTimeVal *time = data;
        return pyglib_float_from_timeval(*time);
}

static PyObject *
_wrap_indicate_listener_get_indicator_property_time(PyGObject *self, 
													PyObject *args,
													PyObject *kwargs)
{
		static char *kwlist[] = { "server", "indicator", "property", "callback",
								  "user_data", NULL };
		ListenerPropertyCbData *listener_property_cb_data;
		gchar *property;
        PyGObject *server, *indicator;
		PyGILState_STATE state;
		PyObject *callback, *user_data = Py_None;
		size_t len;

		state = pyg_gil_state_ensure();

		len = PyTuple_Size(args);

		if (len < 4)
		{
				PyErr_SetString(PyExc_TypeError,
								"IndicateListener.get_property requires at least "
								"3 arguments");
				return NULL;
		}

		if (!PyArg_ParseTupleAndKeywords(args, kwargs,
										 "OOsO|O:IndicateListener.get_property",
										 kwlist,
										 &server, &indicator, &property, &callback, &user_data))
		{
				return NULL;
		}

		if (!PyCallable_Check(callback))
		{
				PyErr_SetString(PyExc_TypeError, "fourth argument must be callable");
				return NULL;
		}

		listener_property_cb_data = g_new0(ListenerPropertyCbData, 1);
		listener_property_cb_data->callback = callback;
		listener_property_cb_data->user_data = user_data;
		listener_property_cb_data->listener = self;
		listener_property_cb_data->pyobject_convert = _convert_time_to_pyobject;

		Py_INCREF(callback);
		Py_INCREF(user_data);

		indicate_listener_get_property_time(INDICATE_LISTENER(self->obj),
											(IndicateListenerServer *)server->obj, 
											(IndicateListenerIndicator *)indicator->obj,
											property,
											(indicate_listener_get_property_time_cb)_listener_get_property_cb,
											listener_property_cb_data);

		Py_INCREF(Py_None);
		pyg_gil_state_release(state);
		return Py_None;
}
#line 1100 "syncindicator.c"


static const PyMethodDef _PyIndicateListener_methods[] = {
    { "display", (PyCFunction)_wrap_indicate_listener_display, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "server_get_type", (PyCFunction)_wrap_indicate_listener_server_get_type, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "server_get_desktop", (PyCFunction)_wrap_indicate_listener_server_get_desktop, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "server_show_interest", (PyCFunction)_wrap_indicate_listener_server_show_interest, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "server_remove_interest", (PyCFunction)_wrap_indicate_listener_server_remove_interest, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "server_check_interest", (PyCFunction)_wrap_indicate_listener_server_check_interest, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_indicator_property", (PyCFunction)_wrap_indicate_listener_get_indicator_property, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_indicator_property_icon", (PyCFunction)_wrap_indicate_listener_get_indicator_property_icon, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_indicator_property_time", (PyCFunction)_wrap_indicate_listener_get_indicator_property_time, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyIndicateListener_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "indicate.Listener",                   /* tp_name */
    sizeof(PyGObject),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyIndicateListener_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_indicate_listener_new,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- IndicateServer ----------- */

static int
_wrap_indicate_server_new(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char* kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
                                     ":indicate.Server.__init__",
                                     kwlist))
        return -1;

    pygobject_constructv(self, 0, NULL);
    if (!self->obj) {
        PyErr_SetString(
            PyExc_RuntimeError, 
            "could not create indicate.Server object");
        return -1;
    }
    return 0;
}

static PyObject *
_wrap_indicate_server_set_desktop_file(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "path", NULL };
    char *path;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"s:Indicate.Server.set_desktop_file", kwlist, &path))
        return NULL;
    
    indicate_server_set_desktop_file(INDICATE_SERVER(self->obj), path);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_set_type(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "type", NULL };
    char *type;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"s:Indicate.Server.set_type", kwlist, &type))
        return NULL;
    
    indicate_server_set_type(INDICATE_SERVER(self->obj), type);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_set_count(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "count", NULL };
    PyObject *py_count = NULL;
    guint count = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O:Indicate.Server.set_count", kwlist, &py_count))
        return NULL;
    if (py_count) {
        if (PyLong_Check(py_count))
            count = PyLong_AsUnsignedLong(py_count);
        else if (PyInt_Check(py_count))
            count = PyInt_AsLong(py_count);
        else
            PyErr_SetString(PyExc_TypeError, "Parameter 'count' must be an int or a long");
        if (PyErr_Occurred())
            return NULL;
    }
    
    indicate_server_set_count(INDICATE_SERVER(self->obj), count);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_show(PyGObject *self)
{
    
    indicate_server_show(INDICATE_SERVER(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_hide(PyGObject *self)
{
    
    indicate_server_hide(INDICATE_SERVER(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_get_next_id(PyGObject *self)
{
    guint ret;

    
    ret = indicate_server_get_next_id(INDICATE_SERVER(self->obj));
    
    return PyLong_FromUnsignedLong(ret);
}

static PyObject *
_wrap_indicate_server_add_indicator(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "indicator", NULL };
    PyGObject *indicator;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O!:Indicate.Server.add_indicator", kwlist, &PyIndicateIndicator_Type, &indicator))
        return NULL;
    
    indicate_server_add_indicator(INDICATE_SERVER(self->obj), INDICATE_INDICATOR(indicator->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_remove_indicator(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "indicator", NULL };
    PyGObject *indicator;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O!:Indicate.Server.remove_indicator", kwlist, &PyIndicateIndicator_Type, &indicator))
        return NULL;
    
    indicate_server_remove_indicator(INDICATE_SERVER(self->obj), INDICATE_INDICATOR(indicator->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_set_default(PyGObject *self)
{
    
    indicate_server_set_default(INDICATE_SERVER(self->obj));
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_check_interest(PyGObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "interest", NULL };
    PyObject *py_interest = NULL;
    int ret;
    IndicateInterests interest;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"O:Indicate.Server.check_interest", kwlist, &py_interest))
        return NULL;
    if (pyg_enum_get_value(INDICATE_TYPE_INTERESTS, py_interest, (gpointer)&interest))
        return NULL;
    
    ret = indicate_server_check_interest(INDICATE_SERVER(self->obj), interest);
    
    return PyBool_FromLong(ret);

}

static PyObject *
_wrap_indicate_server_get_max_indicators(PyGObject *self)
{
    int ret;

    
    ret = indicate_server_get_max_indicators(INDICATE_SERVER(self->obj));
    
    return PyInt_FromLong(ret);
}

static const PyMethodDef _PyIndicateServer_methods[] = {
    { "set_desktop_file", (PyCFunction)_wrap_indicate_server_set_desktop_file, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_type", (PyCFunction)_wrap_indicate_server_set_type, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_count", (PyCFunction)_wrap_indicate_server_set_count, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "show", (PyCFunction)_wrap_indicate_server_show, METH_NOARGS,
      NULL },
    { "hide", (PyCFunction)_wrap_indicate_server_hide, METH_NOARGS,
      NULL },
    { "get_next_id", (PyCFunction)_wrap_indicate_server_get_next_id, METH_NOARGS,
      NULL },
    { "add_indicator", (PyCFunction)_wrap_indicate_server_add_indicator, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "remove_indicator", (PyCFunction)_wrap_indicate_server_remove_indicator, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "set_default", (PyCFunction)_wrap_indicate_server_set_default, METH_NOARGS,
      NULL },
    { "check_interest", (PyCFunction)_wrap_indicate_server_check_interest, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "get_max_indicators", (PyCFunction)_wrap_indicate_server_get_max_indicators, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};

PyTypeObject G_GNUC_INTERNAL PyIndicateServer_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                 /* ob_size */
    "indicate.Server",                   /* tp_name */
    sizeof(PyGObject),          /* tp_basicsize */
    0,                                 /* tp_itemsize */
    /* methods */
    (destructor)0,        /* tp_dealloc */
    (printfunc)0,                      /* tp_print */
    (getattrfunc)0,       /* tp_getattr */
    (setattrfunc)0,       /* tp_setattr */
    (cmpfunc)0,           /* tp_compare */
    (reprfunc)0,             /* tp_repr */
    (PyNumberMethods*)0,     /* tp_as_number */
    (PySequenceMethods*)0, /* tp_as_sequence */
    (PyMappingMethods*)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
    (ternaryfunc)0,          /* tp_call */
    (reprfunc)0,              /* tp_str */
    (getattrofunc)0,     /* tp_getattro */
    (setattrofunc)0,     /* tp_setattro */
    (PyBufferProcs*)0,  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                      /* tp_flags */
    NULL,                        /* Documentation string */
    (traverseproc)0,     /* tp_traverse */
    (inquiry)0,             /* tp_clear */
    (richcmpfunc)0,   /* tp_richcompare */
    offsetof(PyGObject, weakreflist),             /* tp_weaklistoffset */
    (getiterfunc)0,          /* tp_iter */
    (iternextfunc)0,     /* tp_iternext */
    (struct PyMethodDef*)_PyIndicateServer_methods, /* tp_methods */
    (struct PyMemberDef*)0,              /* tp_members */
    (struct PyGetSetDef*)0,  /* tp_getset */
    NULL,                              /* tp_base */
    NULL,                              /* tp_dict */
    (descrgetfunc)0,    /* tp_descr_get */
    (descrsetfunc)0,    /* tp_descr_set */
    offsetof(PyGObject, inst_dict),                 /* tp_dictoffset */
    (initproc)_wrap_indicate_server_new,             /* tp_init */
    (allocfunc)0,           /* tp_alloc */
    (newfunc)0,               /* tp_new */
    (freefunc)0,             /* tp_free */
    (inquiry)0              /* tp_is_gc */
};



/* ----------- functions ----------- */

static PyObject *
_wrap_indicate_listener_server_get_gtype(PyObject *self)
{
    GType ret;

    
    ret = indicate_listener_server_get_gtype();
    
    return pyg_type_wrapper_new(ret);
}

static PyObject *
_wrap_indicate_listener_ref_default(PyObject *self)
{
    IndicateListener *ret;

    
    ret = indicate_listener_ref_default();
    
    /* pygobject_new handles NULL checking */
    return pygobject_new((GObject *)ret);
}

static PyObject *
_wrap_indicate_server_set_dbus_object(PyObject *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { "obj", NULL };
    char *obj;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,"s:indicate_server_set_dbus_object", kwlist, &obj))
        return NULL;
    
    indicate_server_set_dbus_object(obj);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
_wrap_indicate_server_ref_default(PyObject *self)
{
    IndicateServer *ret;

    
    ret = indicate_server_ref_default();
    
    /* pygobject_new handles NULL checking */
    return pygobject_new((GObject *)ret);
}

const PyMethodDef pysyncindicator_functions[] = {
    { "indicate_listener_server_get_gtype", (PyCFunction)_wrap_indicate_listener_server_get_gtype, METH_NOARGS,
      NULL },
    { "indicate_listener_ref_default", (PyCFunction)_wrap_indicate_listener_ref_default, METH_NOARGS,
      NULL },
    { "indicate_server_set_dbus_object", (PyCFunction)_wrap_indicate_server_set_dbus_object, METH_VARARGS|METH_KEYWORDS,
      NULL },
    { "indicate_server_ref_default", (PyCFunction)_wrap_indicate_server_ref_default, METH_NOARGS,
      NULL },
    { NULL, NULL, 0, NULL }
};


/* ----------- enums and flags ----------- */

void
pysyncindicator_add_constants(PyObject *module, const gchar *strip_prefix)
{
#ifdef VERSION
    PyModule_AddStringConstant(module, "__version__", VERSION);
#endif
  pyg_enum_add(module, "Interests", strip_prefix, INDICATE_TYPE_INTERESTS);

  if (PyErr_Occurred())
    PyErr_Print();
}

/* initialise stuff extension classes */
void
pysyncindicator_register_classes(PyObject *d)
{
    PyObject *module;

    if ((module = PyImport_ImportModule("gobject")) != NULL) {
        _PyGObject_Type = (PyTypeObject *)PyObject_GetAttrString(module, "GObject");
        if (_PyGObject_Type == NULL) {
            PyErr_SetString(PyExc_ImportError,
                "cannot import name GObject from gobject");
            return ;
        }
    } else {
        PyErr_SetString(PyExc_ImportError,
            "could not import gobject");
        return ;
    }
    if ((module = PyImport_ImportModule("gtk.gdk")) != NULL) {
        _PyGdkPixbuf_Type = (PyTypeObject *)PyObject_GetAttrString(module, "Pixbuf");
        if (_PyGdkPixbuf_Type == NULL) {
            PyErr_SetString(PyExc_ImportError,
                "cannot import name Pixbuf from gtk.gdk");
            return ;
        }
    } else {
        PyErr_SetString(PyExc_ImportError,
            "could not import gtk.gdk");
        return ;
    }


#line 1535 "syncindicator.c"
    pyg_register_pointer(d, "ListenerServer", INDICATE_TYPE_LISTENER_SERVER, &PyIndicateListenerServer_Type);
    pyg_register_pointer(d, "ListenerIndicator", INDICATE_TYPE_LISTENER_INDICATOR, &PyIndicateListenerIndicator_Type);
    pygobject_register_class(d, "IndicateIndicator", INDICATE_TYPE_INDICATOR, &PyIndicateIndicator_Type, Py_BuildValue("(O)", &PyGObject_Type));
    pyg_set_object_has_new_constructor(INDICATE_TYPE_INDICATOR);
    pygobject_register_class(d, "IndicateListener", INDICATE_TYPE_LISTENER, &PyIndicateListener_Type, Py_BuildValue("(O)", &PyGObject_Type));
    pyg_set_object_has_new_constructor(INDICATE_TYPE_LISTENER);
    pygobject_register_class(d, "IndicateServer", INDICATE_TYPE_SERVER, &PyIndicateServer_Type, Py_BuildValue("(O)", &PyGObject_Type));
    pyg_set_object_has_new_constructor(INDICATE_TYPE_SERVER);
}
