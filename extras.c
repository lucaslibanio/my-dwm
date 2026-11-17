/* Funções extras incluídas pelo config.h:
 *   movestack     (patch movestack)        - move a janela selecionada na pilha
 *   cyclelayout   (patch cyclelayouts)     - percorre todos os layouts
 *   togglefullscr (patch actualfullscreen) - tela cheia de verdade
 */

void
movestack(const Arg *arg)
{
	Client *c = NULL, *p = NULL, *pc = NULL, *i;

	if (!selmon->sel || selmon->sel->isfloating)
		return;
	if (arg->i > 0) {
		/* próxima janela visível e lado a lado */
		for (c = selmon->sel->next; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
		if (!c)
			for (c = selmon->clients; c && (!ISVISIBLE(c) || c->isfloating); c = c->next);
	} else {
		/* janela anterior visível e lado a lado */
		for (i = selmon->clients; i != selmon->sel; i = i->next)
			if (ISVISIBLE(i) && !i->isfloating)
				c = i;
		if (!c)
			for (; i; i = i->next)
				if (ISVISIBLE(i) && !i->isfloating)
					c = i;
	}
	/* acha os antecessores de sel e c */
	for (i = selmon->clients; i && (!p || !pc); i = i->next) {
		if (i->next == selmon->sel)
			p = i;
		if (i->next == c)
			pc = i;
	}
	/* troca sel e c de lugar */
	if (c && c != selmon->sel) {
		Client *temp = selmon->sel->next == c ? selmon->sel : selmon->sel->next;
		selmon->sel->next = c->next == selmon->sel ? c : c->next;
		c->next = temp;

		if (p && p != c)
			p->next = c;
		if (pc && pc != selmon->sel)
			pc->next = selmon->sel;

		if (selmon->sel == selmon->clients)
			selmon->clients = c;
		else if (c == selmon->clients)
			selmon->clients = selmon->sel;

		arrange(selmon);
	}
}

void
cyclelayout(const Arg *arg)
{
	const Layout *l;
	Arg a;

	for (l = layouts; l->symbol && l != selmon->lt[selmon->sellt]; l++);
	if (arg->i > 0) {
		if (l->symbol && (l + 1)->symbol)
			a.v = l + 1;
		else
			a.v = layouts;
	} else {
		if (l != layouts && (l - 1)->symbol)
			a.v = l - 1;
		else
			a.v = &layouts[LENGTH(layouts) - 2];
	}
	setlayout(&a);
}

void
togglefullscr(const Arg *arg)
{
	if (selmon->sel)
		setfullscreen(selmon->sel, !selmon->sel->isfullscreen);
}
