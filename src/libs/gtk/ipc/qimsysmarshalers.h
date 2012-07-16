
#ifndef __qimsys_marshal_MARSHAL_H__
#define __qimsys_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

extern void qimsys_marshal_VOID__UINT64 (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

extern void qimsys_marshal_VOID__STRING_UINT64 (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

extern void qimsys_marshal_VOID__INT_INT_INT_INT (GClosure     *closure,
                                                  GValue       *return_value,
                                                  guint         n_param_values,
                                                  const GValue *param_values,
                                                  gpointer      invocation_hint,
                                                  gpointer      marshal_data);

extern void qimsys_marshal_VOID__STRING_INT_INT_INT (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

G_END_DECLS

#endif /* __qimsys_marshal_MARSHAL_H__ */

