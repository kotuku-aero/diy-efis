/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include "ion.h"

#if defined(DUK_CMDLINE_PRINTALERT_SUPPORT)
#include "duk_print_alert.h"
#endif
#if defined(DUK_CMDLINE_CONSOLE_SUPPORT)
#include "duk_console.h"
#endif
#include "duktape.h"

#if defined(DUK_CMDLINE_DEBUGGER_SUPPORT)
#include "duk_trans_socket.h"
#endif
#define  LINEBUF_SIZE       256

#if defined(DUK_CMDLINE_DEBUGGER_SUPPORT)
static int debugger_reattach = 0;
#endif

static void print_greet_line(ion_context_t *ion)
  {
	stream_printf(ion->console_out, "((o) Duktape%s %d.%d.%d (%s)\n",
	       (int) (DUK_VERSION / 10000),
	       (int) ((DUK_VERSION / 100) % 100),
	       (int) (DUK_VERSION % 100),
	       DUK_GIT_DESCRIBE);
}

static duk_ret_t get_stack_raw(duk_context *ctx, void *udata)
  {
	(void) udata;

	if (!duk_is_object(ctx, -1))
    {
		return 1;
	}
	if (!duk_has_prop_string(ctx, -1, "stack"))
    {
		return 1;
	}
	if (!duk_is_error(ctx, -1)) 
    {
		/* Not an Error instance, don't read "stack". */
		return 1;
	}

	duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
	duk_remove(ctx, -2);
	return 1;
}

/* Print error to stderr and pop error. */
static void print_pop_error(ion_context_t *ion)
  {
	/* Print error objects with a stack trace specially.
	 * Note that getting the stack trace may throw an error
	 * so this also needs to be safe call wrapped.
	 */
	(void) duk_safe_call(ion->ctx, get_stack_raw, NULL /*udata*/, 1 /*nargs*/, 1 /*nrets*/);
	stream_printf(ion->console_err, "%s\n", duk_safe_to_string(ion->ctx, -1));
	duk_pop(ion->ctx);
}

static duk_ret_t wrapped_compile_execute(duk_context *ctx, void *udata)
  {
	const char *src_data;
	duk_size_t src_len;
	int comp_flags;

  duk_memory_functions mem_funcs;
  duk_get_memory_functions(ctx, &mem_funcs);

  ion_context_t *ion = (ion_context_t *)mem_funcs.udata;

	/* XXX: Here it'd be nice to get some stats for the compilation result
	 * when a suitable command line is given (e.g. code size, constant
	 * count, function count.  These are available internally but not through
	 * the public API.
	 */

	/* Use duk_compile_lstring_filename() variant which avoids interning
	 * the source code.  This only really matters for low memory environments.
	 */

	/* [ ... bytecode_filename src_data src_len filename ] */

	src_data = (const char *) duk_require_pointer(ctx, -3);
	src_len = (duk_size_t) duk_require_uint(ctx, -2);

	if (src_data != NULL && src_len >= 2 && src_data[0] == (char) 0xff)
    {
		/* Bytecode. */
		void *buf;
		buf = duk_push_fixed_buffer(ctx, src_len);
		memcpy(buf, (const void *) src_data, src_len);
		duk_load_function(ctx);
	}
  else 
    {
		/* Source code. */
		comp_flags = DUK_COMPILE_SHEBANG;
		duk_compile_lstring_filename(ctx, comp_flags, src_data, src_len);
	}

	/* [ ... bytecode_filename src_data src_len function ] */

	/* Optional bytecode dump. */
	if (duk_is_string(ctx, -4))
    {
    /*
		FILE *f;
		void *bc_ptr;
		duk_size_t bc_len;
		size_t wrote;
		char fnbuf[256];
		const char *filename;

		duk_dup_top(ctx);
		duk_dump_function(ctx);
		bc_ptr = duk_require_buffer(ctx, -1, &bc_len);
		filename = duk_require_string(ctx, -5);
		snprintf(fnbuf, sizeof(fnbuf), "%s", filename);
		fnbuf[sizeof(fnbuf) - 1] = (char) 0;

		f = fopen(fnbuf, "wb");
		if (!f) {
			duk_error(ctx, DUK_ERR_ERROR, "failed to open bytecode output file");
		}
		wrote = fwrite(bc_ptr, 1, (size_t) bc_len, f); 
		(void) fclose(f);
		if (wrote != bc_len) {
			duk_error(ctx, DUK_ERR_ERROR, "failed to write all bytecode");
      
		}

		return 0; */
	}

	duk_push_global_object(ctx);  /* 'this' binding */
	duk_call_method(ctx, 0);

		/*
		 *  In interactive mode, write to stdout so output won't
		 *  interleave as easily.
		 *
		 *  NOTE: the ToString() coercion may fail in some cases;
		 *  for instance, if you evaluate:
		 *
		 *    ( {valueOf: function() {return {}},
		 *       toString: function() {return {}}});
		 *
		 *  The error is:
		 *
		 *    TypeError: coercion to primitive failed
		 *            duk_api.c:1420
		 *
		 *  These are handled now by the caller which also has stack
		 *  trace printing support.  User code can print out errors
		 *  safely using duk_safe_to_string().
		 */

		duk_push_global_stash(ctx);
		duk_get_prop_string(ctx, -1, "dukFormat");
		duk_dup(ctx, -3);
		duk_call(ctx, 1);  /* -> [ ... res stash formatted ] */

		stream_printf(ion->console_out, "= %s\n", duk_to_string(ion->ctx, -1));

	return 0;  /* duk_safe_call() cleans up */
}

#if 0
/*
 *  Execute from file handle etc
 */

static int handle_fh(duk_context *ctx, FILE *f, const char *filename, const char *bytecode_filename)
  {
	char *buf = NULL;
	size_t bufsz;
	size_t bufoff;
	size_t got;
	int rc;
	int retval = -1;

	buf = (char *) malloc(1024);
	if (!buf) {
		goto error;
	}
	bufsz = 1024;
	bufoff = 0;

	/* Read until EOF, avoid fseek/stat because it won't work with stdin. */
	for (;;) {
		size_t avail;

		avail = bufsz - bufoff;
		if (avail < 1024) {
			size_t newsz;
			char *buf_new;
#if 0
			fprintf(stderr, "resizing read buffer: %ld -> %ld\n", (long) bufsz, (long) (bufsz * 2));
#endif
			newsz = bufsz + (bufsz >> 2) + 1024;  /* +25% and some extra */
			buf_new = (char *) realloc(buf, newsz);
			if (!buf_new) {
				goto error;
			}
			buf = buf_new;
			bufsz = newsz;
		}

		avail = bufsz - bufoff;
#if 0
		fprintf(stderr, "reading input: buf=%p bufsz=%ld bufoff=%ld avail=%ld\n",
		        (void *) buf, (long) bufsz, (long) bufoff, (long) avail);
#endif

		got = fread((void *) (buf + bufoff), (size_t) 1, avail, f);
#if 0
		fprintf(stderr, "got=%ld\n", (long) got);
#endif
		if (got == 0) {
			break;
		}
		bufoff += got;
	}

	duk_push_string(ctx, bytecode_filename);
	duk_push_pointer(ctx, (void *) buf);
	duk_push_uint(ctx, (duk_uint_t) bufoff);
	duk_push_string(ctx, filename);

	rc = duk_safe_call(ctx, wrapped_compile_execute, NULL /*udata*/, 4 /*nargs*/, 1 /*nret*/);

	free(buf);
	buf = NULL;

	if (rc != DUK_EXEC_SUCCESS) {
		print_pop_error(ctx, stderr);
		goto error;
	} else {
		duk_pop(ctx);
		retval = 0;
	}
	/* fall thru */

 cleanup:
	if (buf) {
		free(buf);
		buf = NULL;
	}
	return retval;

 error:
	fprintf(stderr, "error in executing file %s\n", filename);
	fflush(stderr);
	goto cleanup;
}

static int handle_file(duk_context *ctx, const char *filename, const char *bytecode_filename) {
	FILE *f = NULL;
	int retval;
	char fnbuf[256];

	// Example of sending an application specific debugger notification.
	duk_push_string(ctx, "DebuggerHandleFile");
	duk_push_string(ctx, filename);
	duk_debugger_notify(ctx, 2);
	snprintf(fnbuf, sizeof(fnbuf), "%s", filename);
	fnbuf[sizeof(fnbuf) - 1] = (char) 0;

	f = fopen(fnbuf, "rb");
	if (!f) {
		fprintf(stderr, "failed to open source file: %s\n", filename);
		fflush(stderr);
		goto error;
	}

	retval = handle_fh(ctx, f, filename, bytecode_filename);

	fclose(f);
	return retval;

 error:
	return -1;
}

static int handle_eval(duk_context *ctx, char *code) {
	int rc;
	int retval = -1;

	duk_push_pointer(ctx, (void *) code);
	duk_push_uint(ctx, (duk_uint_t) strlen(code));
	duk_push_string(ctx, "eval");

	rc = duk_safe_call(ctx, wrapped_compile_execute, NULL /*udata*/, 3 /*nargs*/, 1 /*nret*/);

	if (rc != DUK_EXEC_SUCCESS) {
		print_pop_error(ctx, stderr);
	} else {
		duk_pop(ctx);
		retval = 0;
	}

	return retval;
}
#endif

result_t ion_exec(ion_context_t *ion)
  {
	const char *prompt = "duk> ";
	char *buffer = NULL;
	result_t retval = s_ok;
	int rc;
	bool got_eof = false;

	buffer = (char *) neutron_malloc(LINEBUF_SIZE);
	if (!buffer)
    {
		stream_printf(ion->console_err, "failed to allocated a line buffer\r\n");

    retval = e_not_enough_memory;
		goto done;
	}

	while (!got_eof)
    {
		size_t idx = 0;

		stream_puts(ion->console_out, prompt);

		for (;;)
      {
      char c;
      if(stream_getc(ion->console_in, &c)== e_no_more_information)
			  {
				got_eof = 1;
				break;
			  }
      if (c == '\n')
        {
        stream_puts(ion->console_out, "\r\n");
        break;
        }
      if (c == '\b' || c == 127)
        {
        if (idx == 0)
          continue;
        stream_puts(ion->console_out, "\b \b");
        idx--;
        buffer[idx] = 0;
        }
      else
        {
        stream_putc(ion->console_out, c);
        buffer[idx] = c;
        idx++;
        buffer[idx] = 0;
       if (idx == LINEBUF_SIZE - 1)
          break;
			  }
      }

    if (strcmp(buffer, "exit") == 0)
      {
      retval = s_ok;
      break;
      }

		duk_push_pointer(ion->ctx, (void *) buffer);
		duk_push_uint(ion->ctx, (duk_uint_t) idx);
		duk_push_string(ion->ctx, "input");

    rc = duk_safe_call(ion->ctx, wrapped_compile_execute, NULL /*udata*/, 3 /*nargs*/, 1 /*nret*/);

		if (rc < 0)
      {
			/* in interactive mode, write to stdout */
			print_pop_error(ion);
			retval = e_unexpected;  /* an error 'taints' the execution */
		  }
    else 
      {
			duk_pop(ion->ctx);
		}
	}

 done:
	if (buffer)
    {
		neutron_free(buffer);
		buffer = NULL;
	}

	return retval;
}
