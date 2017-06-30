
/*--------------------------------------------------------------------*/
/*--- vstrace: tracing syscalls for duke nukem 3d   vstrace_main.c ---*/
/*--------------------------------------------------------------------*/

/*
   Copyright (C) 2017 Sebastien Tricaud
      sebastien@honeynet.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU General Public License is contained in the file COPYING.
*/

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_mallocfree.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_libcfile.h"
#include "pub_tool_libcprint.h"

#include "vstrace_syscall_names.h"

static const HChar *vstrace_out_file = "vstrace.out";
static VgFile *fp;

static void vstrace_post_clo_init(void)
{
  fp = VG_(fopen)(vstrace_out_file,
		  VKI_O_CREAT|VKI_O_TRUNC|VKI_O_WRONLY,
                  VKI_S_IRUSR|VKI_S_IWUSR);
  if (!fp) {
    VG_(umsg)("Error: cannot open the file to write syscalls into: %s\n", vstrace_out_file);
    return;
  }
}

static
IRSB* vstrace_instrument ( VgCallbackClosure* closure,
                      IRSB* sbIn,
                      VexGuestLayout* layout, 
                      VexGuestExtents* vge,
                      VexArchInfo* archinfo_host,
                      IRType gWordTy, IRType hWordTy )
{
  Int i;
  IRStmt *st;
  int syscall_id;
  
  i = 0;
  
  for (; i < sbIn->stmts_used; i++) {
    st = sbIn->stmts[i];

    switch(st->tag) {
    case Ist_Put:
      if (st->Ist.Put.offset == 16) {
  	/* RAX */
  	const IRExpr* e;
  	const IRConst* con;
  	e = st->Ist.Put.data;
  	if (e->tag == Iex_Const) {
  	  con = e->Iex.Const.con;
  	  if (con->tag == Ico_U64) {
  	    syscall_id = (int *)con->Ico.U64;
  	  }
  	}
      }
      break;
    }
  } // for (; i < sbIn->stmts_used; i++) {

  
  switch(sbIn->jumpkind) {
  case Ijk_Sys_syscall:
    VG_(fprintf)(fp, "%s\n", syscall_names[syscall_id]);
  break;
  }

  
  return sbIn;
}

static void vstrace_fini(Int exitcode)
{
  VG_(fclose)(fp);
}

static void vstrace_pre_clo_init(void)
{
   VG_(details_name)            ("vstrace");
   VG_(details_version)         (NULL);
   VG_(details_description)     ("Creating a file with syscalls");
   VG_(details_copyright_author)(
      "Copyright (c) 2017 Sebastien Tricaud");
   VG_(details_bug_reports_to)  (VG_BUGS_TO);

   VG_(details_avg_translation_sizeB) ( 275 );

   VG_(basic_tool_funcs)        (vstrace_post_clo_init,
                                 vstrace_instrument,
                                 vstrace_fini);

}

VG_DETERMINE_INTERFACE_VERSION(vstrace_pre_clo_init)

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
