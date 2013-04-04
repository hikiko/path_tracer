/*
Path_tracer - A CPU path tracer

Copyright (C) 2013 Eleni Maria Stea

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

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include "object.h"

Object::Object() {
	ignore = false;
}

Material* Object::get_material() {
	return &material;
}

const Material* Object::get_material() const {
	return &material;
}

bool Object::is_light() const {
	const Color *ke = &get_material()->ke;
	if (ke->x > 0.0 || ke->y > 0.0 || ke->z > 0.0) {
		return true;
	}
	return false;
}
