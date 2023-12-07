/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2023 Balaji Gudala <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-drawshapes
 *
 * FIXME:Describe drawshapes here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! drawshapes ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
//header files included as per requirement
#include <gst/gst.h>
#include <gst/video/video.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gstdrawshapes.h"
#include "gstmacros.h"


//creating catagory for plugin used to debug info
GST_DEBUG_CATEGORY_STATIC (gst_drawshapes_debug);
#define GST_CAT_DEFAULT gst_drawshapes_debug

/* Filter signals and args */
enum
{
	/* FILL ME */
	LAST_SIGNAL
};
//this enum shows all the properties required for plugin
enum
{
	PROP_0,
	PROP_YCO,//y-coordinate of plane
	PROP_XCO,//x-coordinate of plane
	PROP_SHAPE,//the required shape 
	PROP_COLOUR,//the specified colour by user
	/*dimensions of shapes as per requirement*/
	PROP_HEIGHT,
	PROP_WIDTH,
	PROP_RADIUS

};
/* the capabilities of the inputs and outputs.
 * describe the real formats here.
 * defining the static pad templates
 * creates src and sink templates*/
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",GST_PAD_SINK,GST_PAD_ALWAYS,GST_STATIC_CAPS ("ANY"));
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",GST_PAD_SRC,GST_PAD_ALWAYS,GST_STATIC_CAPS ("ANY"));
/* the properties and behaviour of parent class is inherited to our plugin using this macro*/
#define gst_drawshapes_parent_class parent_class
//it sets up the type hierarchy, initializes function pointers,and provides default implementations for object initialization and class initialization.
G_DEFINE_TYPE (GstDrawshapes, gst_drawshapes, GST_TYPE_ELEMENT);
//used to define new gstreamer element
GST_ELEMENT_REGISTER_DEFINE (drawshapes, "drawshapes", GST_RANK_NONE,GST_TYPE_DRAWSHAPES);
/*function signatures*/
static void gst_drawshapes_set_property (GObject * object,guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_drawshapes_get_property (GObject * object,guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_drawshapes_sink_event (GstPad * pad,GstObject * parent, GstEvent * event);
static GstFlowReturn gst_drawshapes_chain (GstPad * pad,GstObject * parent, GstBuffer * buf);


/*  First Entry Point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 * debug category for filtering log messages
 * exchange the string 'Template drawshapes' with your description
 */
static gboolean drawshapes_init (GstPlugin * drawshapes)
{
	//It initializes the debug info through created category variable
	GST_DEBUG_CATEGORY_INIT (gst_drawshapes_debug, "drawshapes",0, "Template drawshapes");
	//register the element whichis defined
	return GST_ELEMENT_REGISTER (drawshapes, drawshapes);
}

/* it  GObject vmethod implementations 
 * initialize the drawshapes's class
 * Second entry point of every plugin:class init
 * It is the GObjectClass  variable is a pointer to the class structure of a GObject-derived class. 
 * It represents the blueprint or definition of a class and contains function pointers, class-specific data. 	 
 * information necessary for working with instances of that class. 
 */
static void gst_drawshapes_class_init (GstDrawshapesClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;

	gobject_class = (GObjectClass *) klass;
	gstelement_class = (GstElementClass *) klass;
	gobject_class->set_property = gst_drawshapes_set_property;
	gobject_class->get_property = gst_drawshapes_get_property;

	g_object_class_install_property (gobject_class, PROP_XCO,g_param_spec_int ("xco", "X-co", "required point on x-axis",
				DRAWSHAPES_XCO_MIN,DRAWSHAPES_XCO_MAX,DRAWSHAPES_XCO_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_YCO,g_param_spec_int ("yco", "Y-co", "required point on y-axis",
				DRAWSHAPES_YCO_MIN,DRAWSHAPES_YCO_MAX,DRAWSHAPES_YCO_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_SHAPE,g_param_spec_string ("shape", "Shape", "the type of shape to generate\n                         1.square - needs only width\n                      2.rectangle - needs width and height\n                        3.triangle-needs width and height \n                         4.circle - needs radius","square", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_COLOUR,g_param_spec_string ("colour", "Colour", "required colour to be keep on shape generated",
				"red", G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_HEIGHT,
			g_param_spec_int ("height", "height", "height of shape on frame",
				DRAWSHAPES_HEIGHT_MIN,DRAWSHAPES_HEIGHT_MAX,DRAWSHAPES_HEIGHT_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_WIDTH,
			g_param_spec_int ("width", "Width", "Width of shape on frame",
				DRAWSHAPES_WIDTH_MIN,DRAWSHAPES_WIDTH_MAX,DRAWSHAPES_WIDTH_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property (gobject_class, PROP_RADIUS,
			g_param_spec_int ("radius", "Radius", "radius of circle to draw on frame",
				DRAWSHAPES_RADIUS_MIN,DRAWSHAPES_RADIUS_MAX,DRAWSHAPES_RADIUS_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));


	gst_element_class_set_details_simple (gstelement_class,
			"Drawshapes",
			"FIXME:Generic",
			"FIXME:Generic Template Element", "Rakesh Konathala<<user@hostname.org>>");

	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (gstelement_class,
			gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_drawshapes_init (GstDrawshapes * filter)
{
	filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
	gst_pad_set_event_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_sink_event));
	gst_pad_set_chain_function (filter->sinkpad,
			GST_DEBUG_FUNCPTR (gst_drawshapes_chain));
	GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

	filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
	GST_PAD_SET_PROXY_CAPS (filter->srcpad);
	gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
	//DEFAULT VALUES ARE ASSIGNED HERE
	filter->xco=DRAWSHAPES_XCO_DEFAULT;
	filter->height=DRAWSHAPES_HEIGHT_DEFAULT;
	filter->width=DRAWSHAPES_WIDTH_DEFAULT;
	filter->yco= DRAWSHAPES_YCO_DEFAULT;
	filter->radius=DRAWSHAPES_RADIUS_DEFAULT;
	strcpy(filter->shape,"square");
	strcpy(filter->colour,"red");

}
static void gst_drawshapes_set_property (GObject * object, guint prop_id,const GValue * value, GParamSpec * pspec)
{
	GstDrawshapes *filter = GST_DRAWSHAPES (object);
	/*THIS FUNCTION GETS VALUE FROM STRUCTURE AND PRINTS ON CONSOLE*/

	switch (prop_id) {
		case PROP_XCO:
			if(g_value_get_int(value)<=0)
			{
				g_print("negative values and out of range values should not give as input\n");
				exit(1);
			}
			else
				filter->xco = g_value_get_int (value);
			break;
		case PROP_YCO:
			if(g_value_get_int(value)<=0)
			{
				g_print("negative values and out of range values should not give as input\n");
				exit(1);
			}
			else
				filter->yco = g_value_get_int (value);
			break;
		case PROP_HEIGHT:
			if(g_value_get_int(value)<=0 )
			{
				g_print("negative values and out of range values should not give as input\n");
				exit(1);
			}
			else
				filter->height = g_value_get_int (value);
			break;
		case PROP_WIDTH:
			if(g_value_get_int(value))
			{
				g_print("negative values and out of range values should not give as input\n");
				exit(1);
			}
			else
				filter->width = g_value_get_int (value);
			break;
		case PROP_RADIUS:
			if(g_value_get_int(value)<0)
			{
				g_print("negative values and out of range values should not give as input\n");
				exit(1);
			}
			else
				filter->radius = g_value_get_int (value);
			break;
		case PROP_SHAPE:
			strcpy(filter->shape,g_value_get_string(value));
			break;
		case PROP_COLOUR:
			strcpy(filter->colour,g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}
static void gst_drawshapes_get_property (GObject * object, guint prop_id,GValue * value, GParamSpec * pspec)
{
	GstDrawshapes *filter = GST_DRAWSHAPES (object);
	/*THIS FUNCTION TO SET VALUE GVALUE STRUCTURE WHICH IS INITIALIZED BEFORE*/

	switch (prop_id) {

		case PROP_XCO:
			g_value_set_int (value ,filter->xco);
			break;
		case PROP_YCO:
			g_value_set_int (value ,filter->yco);
			break;
		case PROP_HEIGHT:
			g_value_set_int (value ,filter->height);
			break;
		case PROP_WIDTH:
			g_value_set_int (value ,filter->width);
			break;
		case PROP_RADIUS:
			g_value_set_int (value ,filter->radius);
			break;
		case PROP_SHAPE:
			g_value_set_string(value,filter->shape);
			break;
		case PROP_COLOUR:
			g_value_set_string(value,filter->colour);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean gst_drawshapes_sink_event (GstPad * pad, GstObject * parent,GstEvent * event)
{
	GstDrawshapes *filter;
	gboolean ret;

	filter = GST_DRAWSHAPES (parent);

	GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
			GST_EVENT_TYPE_NAME (event), event);

	switch (GST_EVENT_TYPE (event)) 
	{
		case GST_EVENT_CAPS:
			{
				GstCaps *caps;

				gst_event_parse_caps (event, &caps);
				/* do something with the caps */

				/* and forward */
				ret = gst_pad_event_default (pad, parent, event);
				break;
			}
		default:
			ret = gst_pad_event_default (pad, parent, event);
			break;
	}
	return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_drawshapes_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
	GstDrawshapes *filter;
	GstVideoInfo video_info;
	GstVideoFrame vframe;

	filter = GST_DRAWSHAPES (parent);
	//printing all the requirements for debugging
	/*g_print("\nxcordinate given is %d\n",filter->xco);
	  g_print("ycoordinate given is %d\n",filter->yco);
	  g_print("the given shape is %s\n",filter->shape);
	  g_print("the given color is %s\n",filter->colour);
	  g_print("the radius given is %d\n",filter->radius);*/
	/*to ensure that you have a writable (modifiable) reference to the buffer*/
	buf=gst_buffer_make_writable(buf);
	if(buf==NULL)
	{
		g_print("failed to make the buffer wriatable\n");
		exit(0);
	}

	if (gst_video_info_from_caps(&video_info, gst_pad_get_current_caps(pad)) && ( gst_video_frame_map(&vframe, &video_info, buf, GST_MAP_WRITE)))
	{
		guint8 *y_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 0); // Y plane
		guint8 *uv_pixels = GST_VIDEO_FRAME_PLANE_DATA(&vframe, 1); // UV plane (interleaved)

		guint y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 0); // Y plane stride
		guint uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(&vframe, 1); // UV plane stride
		guint pixel_stride = 2; // In NV12, UV values are interleaved every 2 bytes

		gint width = GST_VIDEO_FRAME_WIDTH(&vframe);
		gint height = GST_VIDEO_FRAME_HEIGHT(&vframe);
		//gint framesize=GST_VIDEO_FRAME_SIZE(&vframe);
		//g_print("width=%d height=%d framesize=%d\n",width,height,framesize);
		gint y=0,u=0,v=0;//assign colour values as per requirement
		if(!(strcmp("red",filter->colour)))
		{
			y=63;
			u=102;
			v=240;
		}
		if(!(strcmp("green",filter->colour)))
		{
			y=173;
			u=42;
			v=26;
		}
		if(!(strcmp("blue",filter->colour)))
		{
			y=32;
			u=240;
			v=118;
		}
		if((filter->width > 700) && (2*filter->xco<filter->height))
		{
			g_print("boundary out of range\n");
			exit(0);
		}
		if( !(strcmp("square",filter->shape)))
		{
			// Define the coordinates and size of the square
			gint square_center_x =filter->xco ;
			gint square_center_y = filter->yco;
			gint square_size = filter->width;

			// Calculate the coordinates of the top-left and bottom-right corners
			gint square_top_left_x = square_center_x - square_size / 2;
			gint square_top_left_y = square_center_y - square_size / 2;
			gint square_bottom_right_x = square_center_x + square_size / 2;
			gint square_bottom_right_y = square_center_y + square_size / 2;

			GST_DEBUG_OBJECT(filter," width=%d\n",square_size);
			// Loop through the entire frame
			//guint h=square_top_left_y, w=square_bottom_right_x;
			guint h,w;
			for (h=0; h < height; ++h)
			{
				for (w=0; w < width; ++w)
				{
					//this pointer points the exact memory address of pixels to be manipulate
					guint8 *y_pixel = y_pixels + h * y_stride + w; 
					guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;
					// Check if the pixel is on the border of the square
					if (((w == square_top_left_x || w == square_bottom_right_x) &&
								(h >= square_top_left_y && h <= square_bottom_right_y)) ||
							((h == square_top_left_y || h == square_bottom_right_y) &&
							 (w >= square_top_left_x && w <= square_bottom_right_x))) {
						// Set Y, U, and V values to white
						y_pixel[0] = y; // Y component
						uv_pixel[0] = u; // U component (or the first byte of UV)
						uv_pixel[1] = v; // V component (or the second byte of UV)
					}
				}
			}
		}
		if( !(strcmp("rectangle",filter->shape)))
		{
			// Define the coordinates and size of the square
			gint rectangle_center_x =filter->xco ;
			gint rectangle_center_y = filter->yco;
			gint shape_height= filter->height;
			gint shape_width=filter->width;
			GST_DEBUG_OBJECT(filter,"rec height=%d  rec width=%d",shape_height,shape_width);

			// Calculate the coordinates of the top-left and bottom-right corners
			gint rectangle_top_left_x = rectangle_center_x - shape_width / 2;
			gint rectangle_top_left_y = rectangle_center_y - shape_height/ 2;
			gint rectangle_bottom_right_x = rectangle_center_x + shape_width / 2;
			gint rectangle_bottom_right_y = rectangle_center_y + shape_height / 2;

			// Loop through the part of shape to be drawn..
			guint h, w;
			for (h = 0; h < height; ++h)
			{
				for (w =0  ; w < width; ++w)
				{
					guint8 *y_pixel = y_pixels + h * y_stride + w;
					guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;


					// Check if the pixel is on the border of the shape
					if ((((w == rectangle_top_left_x || w == rectangle_bottom_right_x) && (h >= rectangle_top_left_y && h <= rectangle_bottom_right_y))) ||
							(((h == rectangle_top_left_y || h == rectangle_bottom_right_y) && (w >= rectangle_top_left_x && w <= rectangle_bottom_right_x))))
					{
						// Set Y, U, and V values to white
						y_pixel[0] =y; // Y component
						uv_pixel[0] =u; // U component (or the first byte of UV)
						uv_pixel[1] =v; // V component (or the second byte of UV)
					}
				}
			}
		}

		if(!(strcmp("triangle",filter->shape)))
		{
			//define coordinates and size of traingle
			gint triangle_center_x=filter->xco;
			gint triangle_center_y=filter->yco;
			gint triangle_h=filter->height;
			gint triangle_bas=filter->width;
			GST_DEBUG_OBJECT(filter,"height of triangle=%d\n",filter->height);
			GST_DEBUG_OBJECT(filter,"base of triangle=%d\n",filter->width);
			if(!(2*filter->height == filter->width))
			{
				g_warning("improper dimensions of a triangle\n");
				exit(0);
			}
			//calculating vertices of an triangle 
			int x1= triangle_center_x;
			int y1= triangle_center_y - (triangle_h / 2);
			int x2= triangle_center_x - (triangle_bas / 2);
			int y2= triangle_center_y + (triangle_h / 2);
			int x3= triangle_center_x + (triangle_bas / 2);
			int y3= triangle_center_y + (triangle_h / 2);
			GST_DEBUG_OBJECT(filter,"x1=%d y1=%d\nx2=%d y2=%d\nx3=%d y3=%d \n",x1,y1,x2,y2,x3,y3);
			//Calculate the area of the triangle
			gint triangle_area = abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2);
			//loop through the part of shape to be drawn..
			guint h=0,w=0;
			//----------------------------------------------------------------------------------------------------
			for (h = 0; h < height; ++h)
			{
				for (w = 0; w < width; ++w) 
				{
					guint8 *y_pixel = y_pixels + h * y_stride + w;
					guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;

					gint sub_area1 = abs((w * (y1 - y2) + x1 * (y2 - h) + x2 * (h - y1)) / 2);
					gint sub_area2 = abs((w * (y2 - y3) + x2 * (y3 - h) + x3 * (h - y2)) / 2);
					gint sub_area3 = abs((w * (y3 - y1) + x3 * (y1 - h) + x1 * (h - y3)) / 2);
					// Check if the pixel is on the border of the triangle

					if ( ((sub_area1 == 0 || sub_area2 == 0 || sub_area3 == 0) && (sub_area1 + sub_area2 + sub_area3 != triangle_area))  && ((h<=y2 && h>=y1 ) && ((w>=x2) && (w<=x3) )))
					{

						y_pixel[0] = y; // Y component
						uv_pixel[0] =u; // U component (or the first byte of UV)
						uv_pixel[1] =v; // V component (or the second byte of UV)
					}
				}
			}
			//--------------------------------------------------------------------------------------------

		}
		if(!(strcmp("circle",filter->shape)))
		{
			//define coordinates and size of traingle
			gint circle_center_x=filter->xco;
			gint circle_center_y=filter->yco;
			gint radius=filter->radius;
			GST_DEBUG_OBJECT(filter,"radius is %d center (%d %d) ",radius,circle_center_x,circle_center_y);

			//loop through coordinates over circle
			gint h,w,d1,d2;
			for (h = circle_center_y - radius; h <= circle_center_y+radius; ++h)
			{
				for (w = circle_center_x - radius; w <= circle_center_x + radius; ++w)
				{
					guint8 *y_pixel = y_pixels + h * y_stride + w;
					guint8 *uv_pixel = uv_pixels + h / 2 * uv_stride + (w / 2) * pixel_stride;
					// Calculate the distance between the current pixel and the circle's center
					d1 = w - circle_center_x;
					d2 = h - circle_center_y;
					gint distance = sqrt(d1 * d1 + d2 * d2);

					// Check if the pixel is on the border of the circle
					if ((fabs(distance - radius)) <= 1.0)
					{
						// Set Y, U, and V values to white
						y_pixel[0] = y; // Y component
						uv_pixel[0] = u; // U component (or the first byte of UV)
						uv_pixel[1] =v; // V component (or the second byte of UV)
					}
				}

			}

		}


		gst_video_frame_unmap(&vframe);
	}
	else
	{
		g_print("failed to map frame info\n");
		exit(0);
	}
	/* just push out the incoming buffer without touching it */
	return gst_pad_push (filter->srcpad, buf);

}


/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstdrawshapes"
#endif

/* gstreamer looks for this structure to register drawshapess
 *
 * exchange the string 'Template drawshapes' with your drawshapes description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		drawshapes,
		"drawshapes",
		drawshapes_init,
		PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
