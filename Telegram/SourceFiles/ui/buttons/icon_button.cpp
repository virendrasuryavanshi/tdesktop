/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#include "stdafx.h"
#include "ui/buttons/icon_button.h"

namespace Ui {

IconButton::IconButton(QWidget *parent, const style::IconButton &st) : Button(parent)
, _st(st) {
	resize(_st.width, _st.height);
	setCursor(style::cur_pointer);
}

void IconButton::setIcon(const style::icon *icon) {
	_iconOverride = icon;
	update();
}

void IconButton::paintEvent(QPaintEvent *e) {
	Painter p(this);

	auto over = _a_over.current(getms(), (_state & StateOver) ? 1. : 0.);
	p.setOpacity(over * _st.overOpacity + (1. - over) * _st.opacity);

	auto icon = (_iconOverride ? _iconOverride : &_st.icon);
	auto position = (_state & StateDown) ? _st.downIconPosition : _st.iconPosition;
	if (position.x() < 0) {
		position.setX((width() - icon->width()) / 2);
	}
	icon->paint(p, position, width());
}

void IconButton::onStateChanged(int oldState, ButtonStateChangeSource source) {
	auto over = (_state & StateOver);
	if (over != (oldState & StateOver)) {
		auto from = over ? 0. : 1.;
		auto to = over ? 1. : 0.;
		_a_over.start([this] { update(); }, from, to, _st.duration);
	}
}

MaskButton::MaskButton(QWidget *parent, const style::MaskButton &st) : Button(parent)
, _st(st) {
	resize(_st.width, _st.height);
	setCursor(style::cur_pointer);
	setAttribute(Qt::WA_OpaquePaintEvent);
}

void MaskButton::onStateChanged(int oldState, ButtonStateChangeSource source) {
	auto over = (_state & StateOver);
	if (over != (oldState & StateOver)) {
		auto from = over ? _st.iconBg->c : _st.iconBgOver->c;
		auto to = over ? _st.iconBgOver->c : _st.iconBg->c;
		_a_iconBg.start([this] { update(); }, from, to, _st.duration);
	}
}

void MaskButton::paintEvent(QPaintEvent *e) {
	Painter p(this);

	auto clip = e->rect();
	auto position = (_state & StateDown) ? _st.downIconPosition : _st.iconPosition;
	if (position.x() < 0) {
		position.setX((width() - _st.icon.width()) / 2);
	}
	if (position.y() < 0) {
		position.setY((height() - _st.icon.height()) / 2);
	}
	auto icon = myrtlrect(position.x(), position.y(), _st.icon.width(), _st.icon.height());
	if (!icon.contains(clip)) {
		p.fillRect(clip, _st.bg);
	}
	if (icon.intersects(clip)) {
		if (_a_iconBg.animating(getms())) {
			p.fillRect(icon.intersected(clip), _a_iconBg.current());
		} else {
			p.fillRect(icon.intersected(clip), (_state & StateOver) ? _st.iconBgOver : _st.iconBg);
		}
		_st.icon.paint(p, position, width());
	}
}

} // namespace Ui
