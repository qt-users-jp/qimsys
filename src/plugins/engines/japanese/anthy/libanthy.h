/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2009-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef LIBANTHY_H
#define LIBANTHY_H

/* constants */
#define NTH_UNCONVERTED_CANDIDATE -1
#define NTH_KATAKANA_CANDIDATE -2
#define NTH_HIRAGANA_CANDIDATE -3
#define NTH_HALFKANA_CANDIDATE -4
/* encoding constants */
#define ANTHY_COMPILED_ENCODING 0
#define ANTHY_EUC_JP_ENCODING 1
#define ANTHY_UTF8_ENCODING 2
/* reconvert mode */
#define ANTHY_RECONVERT_AUTO 0
#define ANTHY_RECONVERT_DISABLE 1
#define ANTHY_RECONVERT_ALWAYS 2

class LibAnthy
{
public:
    LibAnthy();
    ~LibAnthy();

    bool isAvailable() const;

    /* Data types */
    struct anthy_conv_stat {
        int nr_segment;
    };

    struct anthy_segment_stat {
        int nr_candidate;
        int seg_len;
    };

    struct anthy_prediction_stat {
        int nr_prediction;
    };

    typedef struct anthy_context *anthy_context_t;

    /* Configuration */
    int (*anthy_init)(void);
    void (*anthy_quit)(void);
// void (*anthy_conf_override)( const char*, const char *);
// int (*anthy_set_personality)( const char *);

    /* Context management */
    anthy_context_t (*anthy_create_context)(void);
    void (*anthy_reset_context)(anthy_context_t);
    void (*anthy_release_context)(anthy_context_t);

    /* Conversion control */
    int (*anthy_set_string)(anthy_context_t, const char*);
    /* nth,resize ie. +1,-1 */
    void (*anthy_resize_segment)(anthy_context_t, int, int);
    int (*anthy_get_stat)(anthy_context_t, struct anthy_conv_stat*);
    int (*anthy_get_segment_stat)(anthy_context_t, int, struct anthy_segment_stat*);
    /* context,nth segment,nth candidate,buffer,buffer len */
    int (*anthy_get_segment)(anthy_context_t, int, int, char*, int);
    /* 一文節ごとにコミットする */
    int (*anthy_commit_segment)(anthy_context_t, int, int);

    /* Prediction */
    int (*anthy_set_prediction_string)(anthy_context_t, const char*);
    int (*anthy_get_prediction_stat)(anthy_context_t, struct anthy_prediction_stat*);
    /* context, nth prediction, buffer, buffer len*/
    int (*anthy_get_prediction)(anthy_context_t, int, char*, int);
    int (*anthy_commit_prediction)(anthy_context_t, int);

#if 0
    /* Etc */
    void anthy_print_context(anthy_context_t);

    const char *anthy_get_version_string(void);
    typedef void (*anthy_logger)(int level, const char *);
    void anthy_set_logger(anthy_logger , int level);

    /* experimental and unstable */
    int anthy_context_set_encoding(anthy_context_t ac, int encoding);
    int anthy_set_reconversion_mode(anthy_context_t ac, int mode);
#endif

private:
    class Private;
    Private *d;
};

#endif//LIBANTHY_H
