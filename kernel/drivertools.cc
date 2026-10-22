/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Jannis Harder <jix@yosyshq.com> <me@jix.one>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#अन्तर्भाव "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit अचर &bit)
{
	यदि (bit.is_wire())
		*एतत् = DriveBitWire(bit.wire, bit.offset);
	अन्यथा
		*एतत् = bit.data;
}

शून्य DriveBit::merge(DriveBit अचर &other)
{
	यदि (other.type_ == DriveType::NONE)
		फर्काऊ;
	यदि (type_ == DriveType::NONE) {
		*एतत् = other;
		फर्काऊ;
	}
	यदि (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*एतत्));
		*एतत् = std::move(multi);
	}
	multiple().merge(other);
}


शून्य DriveBitMultiple::merge(DriveBit अचर &single)
{
	यदि (single.type() == DriveType::NONE)
		फर्काऊ;
	यदि (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		फर्काऊ;
	}
	multiple_.emplace(single);
}

शून्य DriveBitMultiple::merge(DriveBit &&single)
{
	यदि (single.type() == DriveType::NONE)
		फर्काऊ;
	यदि (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		फर्काऊ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::चालक[](पूर्णाङ्क i) अचर
{
	DriveBitMultiple result;
	लागि (स्वतः अचर &single : multiple_)
		result.merge(single[i]);
	फर्काऊ result;
}


तार्किक DriveChunkWire::can_append(DriveBitWire अचर &bit) अचर
{
	फर्काऊ bit.wire == wire && bit.offset == offset + width;
}

तार्किक DriveChunkWire::try_append(DriveBitWire अचर &bit)
{
	यदि (!can_append(bit))
		फर्काऊ असत्य;
	width += 1;
	फर्काऊ सत्य;
}

तार्किक DriveChunkWire::try_append(DriveChunkWire अचर &chunk)
{
	यदि (chunk.wire != wire || chunk.offset != offset + width)
		फर्काऊ असत्य;
	width += chunk.width;
	फर्काऊ सत्य;
}

तार्किक DriveChunkPort::can_append(DriveBitPort अचर &bit) अचर
{
	फर्काऊ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

तार्किक DriveChunkPort::try_append(DriveBitPort अचर &bit)
{
	यदि (!can_append(bit))
		फर्काऊ असत्य;
	width += 1;
	फर्काऊ सत्य;
}

तार्किक DriveChunkPort::try_append(DriveChunkPort अचर &chunk)
{
	यदि (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		फर्काऊ असत्य;
	width += chunk.width;
	फर्काऊ सत्य;
}

तार्किक DriveChunkMarker::can_append(DriveBitMarker अचर &bit) अचर
{
	फर्काऊ bit.marker == marker && bit.offset == offset + width;
}

तार्किक DriveChunkMarker::try_append(DriveBitMarker अचर &bit)
{
	यदि (!can_append(bit))
		फर्काऊ असत्य;
	width += 1;
	फर्काऊ सत्य;
}

तार्किक DriveChunkMarker::try_append(DriveChunkMarker अचर &chunk)
{
	यदि (chunk.marker != marker || chunk.offset != offset + width)
		फर्काऊ असत्य;
	width += chunk.width;
	फर्काऊ सत्य;
}


तार्किक DriveChunkMultiple::can_append(DriveBitMultiple अचर &bit) अचर
{
	यदि (bit.multiple().size() != multiple_.size())
		फर्काऊ असत्य;

	पूर्णाङ्क const_drivers = 0;
	लागि (DriveChunk अचर &single : multiple_)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		फर्काऊ असत्य;

	लागि (DriveBit अचर &single : bit.multiple())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		फर्काऊ असत्य;

	लागि (DriveChunk अचर &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} रोक;
			पक्ष DriveType::WIRE: {
				स्वतः अचर &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				यदि (!bit.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			पक्ष DriveType::PORT: {
				स्वतः अचर &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				यदि (!bit.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			पक्ष DriveType::MARKER: {
				स्वतः अचर &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				यदि (!bit.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			स्वभाव:
				फर्काऊ असत्य;
		}
	}

	फर्काऊ सत्य;
}

तार्किक DriveChunkMultiple::can_append(DriveChunkMultiple अचर &chunk) अचर
{
	यदि (chunk.multiple().size() != multiple_.size())
		फर्काऊ असत्य;

	पूर्णाङ्क const_drivers = 0;
	लागि (DriveChunk अचर &single : multiple_)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		फर्काऊ असत्य;

	लागि (DriveChunk अचर &single : chunk.multiple())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		फर्काऊ असत्य;

	लागि (DriveChunk अचर &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} रोक;
			पक्ष DriveType::WIRE: {
				स्वतः अचर &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				यदि (!chunk.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			पक्ष DriveType::PORT: {
				स्वतः अचर &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				यदि (!chunk.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			पक्ष DriveType::MARKER: {
				स्वतः अचर &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				यदि (!chunk.multiple().गन्ती(next))
					फर्काऊ असत्य;
			} रोक;
			स्वभाव:
				फर्काऊ असत्य;
		}
	}

	फर्काऊ सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveBitMultiple अचर &bit)
{
	यदि (!can_append(bit))
		फर्काऊ असत्य;
	width_ += 1;
	State constant;

	लागि (DriveBit अचर &single : bit.multiple())
		यदि (single.is_constant())
			constant = single.constant();

	लागि (DriveChunk &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} रोक;
			पक्ष DriveType::WIRE: {
				single.wire().width += 1;
			} रोक;
			पक्ष DriveType::PORT: {
				single.port().width += 1;
			} रोक;
			पक्ष DriveType::MARKER: {
				single.marker().width += 1;
			} रोक;
			स्वभाव:
				log_abort();
		}
	}
	फर्काऊ सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveChunkMultiple अचर &chunk)
{
	यदि (!can_append(chunk))
		फर्काऊ असत्य;
	पूर्णाङ्क width = chunk.size();
	width_ += width;
	Const constant;

	लागि (DriveChunk अचर &single : chunk.multiple())
		यदि (single.is_constant())
			constant = single.constant();

	लागि (DriveChunk &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(constant);
			} रोक;
			पक्ष DriveType::WIRE: {
				single.wire().width += width;
			} रोक;
			पक्ष DriveType::PORT: {
				single.port().width += width;
			} रोक;
			पक्ष DriveType::MARKER: {
				single.marker().width += width;
			} रोक;
			स्वभाव:
				log_abort();
		}
	}
	फर्काऊ सत्य;
}

तार्किक DriveChunk::can_append(DriveBit अचर &bit) अचर
{
	यदि (size() == 0)
		फर्काऊ सत्य;
	यदि (bit.type() != type_)
		फर्काऊ असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			फर्काऊ सत्य;
		पक्ष DriveType::CONSTANT:
			फर्काऊ सत्य;
		पक्ष DriveType::WIRE:
			फर्काऊ wire_.can_append(bit.wire());
		पक्ष DriveType::PORT:
			फर्काऊ port_.can_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			फर्काऊ multiple_.can_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}

तार्किक DriveChunk::try_append(DriveBit अचर &bit)
{
	यदि (size() == 0)
		*एतत् = bit;
	यदि (bit.type() != type_)
		फर्काऊ असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += 1;
			फर्काऊ सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			फर्काऊ सत्य;
		पक्ष DriveType::WIRE:
			फर्काऊ wire_.try_append(bit.wire());
		पक्ष DriveType::PORT:
			फर्काऊ port_.try_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			फर्काऊ multiple_.try_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}


तार्किक DriveChunk::try_append(DriveChunk अचर &chunk)
{
	यदि (size() == 0)
		*एतत् = chunk;
	यदि (chunk.type_ != type_)
		फर्काऊ असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += chunk.none_;
			फर्काऊ सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			फर्काऊ सत्य;
		पक्ष DriveType::WIRE:
			फर्काऊ wire_.try_append(chunk.wire());
		पक्ष DriveType::PORT:
			फर्काऊ port_.try_append(chunk.port());
		पक्ष DriveType::MARKER:
			फर्काऊ marker_.try_append(chunk.marker());
		पक्ष DriveType::MULTIPLE:
			फर्काऊ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

शून्य DriveSpec::append(DriveBit अचर &bit)
{
	hash_ = 0;
	यदि (!packed()) {
		bits_.थप_back(bit);
		width_ += 1;
		फर्काऊ;
	}

	यदि (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

शून्य DriveSpec::append(DriveChunk अचर &chunk)
{
	hash_ = 0;
	pack();
	यदि (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

शून्य DriveSpec::pack() अचर {
	यदि (bits_.empty())
		फर्काऊ;
	std::vector<DriveBit> bits(std::move(bits_));
	लागि (स्वतः &bit : bits)
		यदि (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

शून्य DriveSpec::unpack() अचर {
	यदि (chunks_.empty())
		फर्काऊ;
	लागि (स्वतः &chunk : chunks_)
	{
		लागि (पूर्णाङ्क i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

शून्य DriveSpec::compute_width()
{
	width_ = 0;
	लागि (स्वतः अचर &chunk : chunks_)
		width_ += chunk.size();
}

शून्य DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	यदि (first_edges.emplace(src, dst).first->second == dst)
		फर्काऊ;
	यदि (second_edges.emplace(src, dst).first->second == dst)
		फर्काऊ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::झिक_edge(DriveBitId src)
{
	// TODO unused I think?
	स्वतः found_more = more_edges.find(src);
	यदि (found_more != more_edges.end()) {
		स्वतः result = found_more->second.झिक();
		यदि (found_more->second.empty())
			more_edges.erase(found_more);
		फर्काऊ result;
	}

	स्वतः found_second = second_edges.find(src);
	यदि (found_second != second_edges.end()) {
		स्वतः result = found_second->second;
		second_edges.erase(found_second);
		फर्काऊ result;
	}

	स्वतः found_first = first_edges.find(src);
	यदि (found_first != first_edges.end()) {
		स्वतः result = found_first->second;
		first_edges.erase(found_first);
		फर्काऊ result;
	}

	फर्काऊ DriveBitId();
}

शून्य DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

तार्किक DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	फर्काऊ first_edges.गन्ती(src);
}

पूर्णाङ्क DriverMap::DriveBitGraph::गन्ती(DriveBitId src)
{
	यदि (!first_edges.गन्ती(src))
		फर्काऊ 0;
	यदि (!second_edges.गन्ती(src))
		फर्काऊ 1;
	स्वतः found = more_edges.find(src);
	यदि (found == more_edges.end())
		फर्काऊ 2;
	फर्काऊ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णाङ्क index)
{
	यदि (index == 0)
		फर्काऊ first_edges.at(src);
	अन्यथा यदि (index == 1)
		फर्काऊ second_edges.at(src);
	अन्यथा
		फर्काऊ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit अचर &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			फर्काऊ BitMode::NONE;
		पक्ष DriveType::CONSTANT:
			// TODO how to handle Sx here?
			फर्काऊ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		पक्ष DriveType::WIRE: {
			स्वतः अचर &wire = bit.wire();
			तार्किक driver = wire.wire->port_आगत;
			तार्किक driven = wire.wire->port_output;

			यदि (driver && !driven)
				फर्काऊ BitMode::DRIVER;
			अन्यथा यदि (driven && !driver)
				फर्काऊ BitMode::DRIVEN;
			अन्यथा यदि (driver && driven)
				फर्काऊ BitMode::TRISTATE;
			अन्यथा
				फर्काऊ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		पक्ष DriveType::PORT: {
			स्वतः अचर &port = bit.port();
			तार्किक driver = celltypes.cell_output(port.cell->type, port.port);
			तार्किक driven = celltypes.cell_आगत(port.cell->type, port.port);
			यदि (driver && !driven)
				फर्काऊ BitMode::DRIVER;
			अन्यथा यदि (driven && !driver)
				फर्काऊ BitMode::DRIVEN_UNIQUE;
			अन्यथा
				फर्काऊ BitMode::TRISTATE;
		}
		पक्ष DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		स्वभाव:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit अचर &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			फर्काऊ -1;
		पक्ष DriveType::CONSTANT:
			फर्काऊ (पूर्णाङ्क)bit.constant();
		पक्ष DriveType::WIRE: {
			स्वतः अचर &wire_bit = bit.wire();
			पूर्णाङ्क offset = next_offset;
			स्वतः insertion = wire_offsets.emplace(wire_bit.wire, offset);
			यदि (insertion.second) {
				यदि (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			फर्काऊ insertion.first->second.id + wire_bit.offset;
		}
		पक्ष DriveType::PORT: {
			स्वतः अचर &port_bit = bit.port();
			स्वतः साँचो = std::make_pair(port_bit.cell, port_bit.port);
			पूर्णाङ्क offset = next_offset;
			स्वतः insertion = port_offsets.emplace(साँचो, offset);
			यदि (insertion.second) {
				पूर्णाङ्क width = port_bit.cell->connections().at(port_bit.port).size();
				यदि (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			फर्काऊ insertion.first->second.id + port_bit.offset;
		}
		स्वभाव:
			log_assert(असत्य && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	स्वतः found_isolated = isolated_drive_bits.find(id);
	यदि (found_isolated != isolated_drive_bits.end())
		फर्काऊ found_isolated->second;

	स्वतः found = drive_bits.upper_bound(id);
	यदि (found == drive_bits.begin()) {
		फर्काऊ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	यदि (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} अन्यथा {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	फर्काऊ result;
}

शून्य DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	यदि (driven_id == driver_id)
		फर्काऊ;

	same_driver.merge(driven_id, driver_id);

	लागि (पूर्णाङ्क i = 0, end = connected_drivers.गन्ती(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	लागि (पूर्णाङ्क i = 0, end = connected_undirected.गन्ती(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

शून्य DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

शून्य DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

शून्य DriverMap::add(Module *module)
{
	लागि (स्वतः अचर &conn : module->connections())
		add(conn.first, conn.second);

	लागि (स्वतः cell : module->cells())
		लागि (स्वतः अचर &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
शून्य DriverMap::add(DriveBit अचर &a, DriveBit अचर &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	यदि (a_id == b_id)
		फर्काऊ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	यदि (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	अन्यथा यदि (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	अन्यथा यदि (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	अन्यथा यदि (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	अन्यथा यदि (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	अन्यथा यदि (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	अन्यथा
		connect_undirected(a_id, b_id);

	फर्काऊ;
}

// Specialized version that avoids unpacking
शून्य DriverMap::add(SigSpec अचर &a, SigSpec अचर &b)
{
	log_assert(a.size() == b.size());
	स्वतः अचर &a_chunks = a.chunks();
	स्वतः अचर &b_chunks = b.chunks();

	स्वतः a_chunk = a_chunks.begin();
	स्वतः a_end = a_chunks.end();
	पूर्णाङ्क a_offset = 0;

	स्वतः b_chunk = b_chunks.begin();
	पूर्णाङ्क b_offset = 0;

	SigChunk tmp_a, tmp_b;
	जबसम्म (a_chunk != a_end) {
		पूर्णाङ्क a_width = a_chunk->width - a_offset;
		यदि (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			जारी;
		}
		पूर्णाङ्क b_width = b_chunk->width - b_offset;
		यदि (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			जारी;
		}
		पूर्णाङ्क width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk अचर &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk अचर &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

शून्य DriverMap::add_port(Cell *cell, IdString port, SigSpec अचर &b)
{
	पूर्णाङ्क offset = 0;
	लागि (स्वतः अचर &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

शून्य DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, पूर्णाङ्क> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	लागि (पूर्णाङ्क pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		यदि (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		यदि (connected_drivers.contains(current))
			drivers.emplace(current);

		पूर्णाङ्क undirected_driver_गन्ती = connected_undirected.गन्ती(current);

		लागि (पूर्णाङ्क i = 0; i != undirected_driver_गन्ती; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	यदि (drivers.empty())
		लागि (स्वतः seen_id : seen)
			drivers.emplace(seen_id);

	लागि (स्वतः driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		लागि (पूर्णाङ्क pos = 0; pos < GetSize(distance); ++pos) {
			स्वतः current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णाङ्क undirected_driver_गन्ती = connected_undirected.गन्ती(current);

			लागि (पूर्णाङ्क i = 0; i != undirected_driver_गन्ती; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				स्वतः emplaced = distance.emplace(next, current_it->second + 1);
				यदि (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	लागि (स्वतः seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::चालक()(DriveBit अचर &bit)
{
	यदि (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		फर्काऊ bit;
	यदि (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		लागि (स्वतः अचर &inner : bit.multiple().multiple())
			result.merge((*एतत्)(inner));
		फर्काऊ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	यदि (mode == BitMode::KEEP && bit_repr_id != bit_id)
		फर्काऊ bit_repr;

	पूर्णाङ्क implicit_driver_गन्ती = connected_drivers.गन्ती(bit_repr_id);
	यदि (connected_undirected.contains(bit_repr_id) && !oriented_present.गन्ती(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	यदि (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	लागि (पूर्णाङ्क i = 0; i != implicit_driver_गन्ती; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णाङ्क oriented_driver_गन्ती = connected_oriented.गन्ती(bit_repr_id);
	लागि (पूर्णाङ्क i = 0; i != oriented_driver_गन्ती; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	फर्काऊ driver;
}

DriveSpec DriverMap::चालक()(DriveSpec spec)
{
	DriveSpec result;

	लागि (पूर्णाङ्क i = 0, width = spec.size(); i != width; ++i)
		result.append((*एतत्)(spec[i]));

	फर्काऊ result;
}

std::string log_signal(DriveChunkWire अचर &chunk)
{
	std::string id = chunk.wire->name.unescape();
	यदि (chunk.is_whole())
		फर्काऊ id;
	यदि (chunk.width == 1)
		फर्काऊ stringf("%s [%d]", id, chunk.offset);
	फर्काऊ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort अचर &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	यदि (chunk.is_whole())
		फर्काऊ stringf("%s <%s>", cell_id, port_id);
	यदि (chunk.width == 1)
		फर्काऊ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	फर्काऊ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker अचर &chunk)
{
	यदि (chunk.width == 1)
		फर्काऊ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	फर्काऊ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk अचर &chunk)
{
	विकल्प (chunk.type())
	{
		पक्ष DriveType::NONE:
			फर्काऊ stringf("<none x%d>", chunk.size());
		पक्ष DriveType::CONSTANT:
			फर्काऊ log_const(chunk.constant());
		पक्ष DriveType::WIRE:
			फर्काऊ log_signal(chunk.wire());
		पक्ष DriveType::PORT:
			फर्काऊ log_signal(chunk.port());
		पक्ष DriveType::MARKER:
			फर्काऊ log_signal(chunk.marker());
		पक्ष DriveType::MULTIPLE: {
			std::string str = "<multiple";
			अचर वर्ण *sep = " ";
			लागि (स्वतः अचर &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			फर्काऊ str;
		}
		स्वभाव:
			log_abort();
	}
}

std::string log_signal(DriveSpec अचर &spec)
{
	स्वतः &chunks = spec.chunks();
	यदि (chunks.empty())
		फर्काऊ "{}";
	यदि (chunks.size() == 1)
		फर्काऊ log_signal(chunks[0]);

	std::string str;
	अचर वर्ण *sep = "{ ";

	लागि (स्वतः i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	फर्काऊ str;
}

YOSYS_NAMESPACE_END
