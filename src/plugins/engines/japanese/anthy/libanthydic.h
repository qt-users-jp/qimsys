/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
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

#ifndef LIBANTHYDIC_H
#define LIBANTHYDIC_H

#define ANTHY_DIC_UTIL_OK 0
/* 失敗 / 登録に失敗した */
#define ANTHY_DIC_UTIL_ERROR -1
/* 同じ単語が登録してあった、頻度だけを上書き */
#define ANTHY_DIC_UTIL_DUPLICATE -2
/* 辞書が存在しない */
#define ANTHY_DIC_UTIL_INVALID -3

class LibAnthyDic
{
public:
    LibAnthyDic();
    ~LibAnthyDic();

    bool isAvailable() const;

    void (*anthy_dic_util_init)(void);
    void (*anthy_dic_util_quit)(void);
    void (*anthy_dic_util_set_personality)(const char*);
    const char*(*anthy_dic_util_get_anthydir)(void);
    int (*anthy_dic_util_set_encoding)(int);

    void (*anthy_priv_dic_delete)(void);
    int (*anthy_priv_dic_select_first_entry)(void);
    int (*anthy_priv_dic_select_next_entry)(void);

    char*(*anthy_priv_dic_get_index)(char *buf, int len);
    int (*anthy_priv_dic_get_freq)(void);
    char*(*anthy_priv_dic_get_wtype)(char *buf, int len);
    char*(*anthy_priv_dic_get_word)(char *buf, int len);

    int (*anthy_priv_dic_add_entry)(const char *yomi, const char *word, const char *wt, int freq);

private:
    class Private;
    Private *d;
};

#endif // LIBANTHYDIC_H
