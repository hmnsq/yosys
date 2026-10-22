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

DriveBit::DriveBit(SigBit स्थिरांक &bit)
{
	जर (bit.is_wire())
		*हे = DriveBitWire(bit.wire, bit.offset);
	अन्यथा
		*हे = bit.data;
}

शून्य DriveBit::merge(DriveBit स्थिरांक &other)
{
	जर (other.type_ == DriveType::NONE)
		परत;
	जर (type_ == DriveType::NONE) {
		*हे = other;
		परत;
	}
	जर (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*हे));
		*हे = std::move(multi);
	}
	multiple().merge(other);
}


शून्य DriveBitMultiple::merge(DriveBit स्थिरांक &single)
{
	जर (single.type() == DriveType::NONE)
		परत;
	जर (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		परत;
	}
	multiple_.emplace(single);
}

शून्य DriveBitMultiple::merge(DriveBit &&single)
{
	जर (single.type() == DriveType::NONE)
		परत;
	जर (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		परत;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::चालक[](पूर्णांक i) स्थिरांक
{
	DriveBitMultiple result;
	साठी (स्वतः स्थिरांक &single : multiple_)
		result.merge(single[i]);
	परत result;
}


तार्किक DriveChunkWire::can_append(DriveBitWire स्थिरांक &bit) स्थिरांक
{
	परत bit.wire == wire && bit.offset == offset + width;
}

तार्किक DriveChunkWire::try_append(DriveBitWire स्थिरांक &bit)
{
	जर (!can_append(bit))
		परत असत्य;
	width += 1;
	परत सत्य;
}

तार्किक DriveChunkWire::try_append(DriveChunkWire स्थिरांक &chunk)
{
	जर (chunk.wire != wire || chunk.offset != offset + width)
		परत असत्य;
	width += chunk.width;
	परत सत्य;
}

तार्किक DriveChunkPort::can_append(DriveBitPort स्थिरांक &bit) स्थिरांक
{
	परत bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

तार्किक DriveChunkPort::try_append(DriveBitPort स्थिरांक &bit)
{
	जर (!can_append(bit))
		परत असत्य;
	width += 1;
	परत सत्य;
}

तार्किक DriveChunkPort::try_append(DriveChunkPort स्थिरांक &chunk)
{
	जर (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		परत असत्य;
	width += chunk.width;
	परत सत्य;
}

तार्किक DriveChunkMarker::can_append(DriveBitMarker स्थिरांक &bit) स्थिरांक
{
	परत bit.marker == marker && bit.offset == offset + width;
}

तार्किक DriveChunkMarker::try_append(DriveBitMarker स्थिरांक &bit)
{
	जर (!can_append(bit))
		परत असत्य;
	width += 1;
	परत सत्य;
}

तार्किक DriveChunkMarker::try_append(DriveChunkMarker स्थिरांक &chunk)
{
	जर (chunk.marker != marker || chunk.offset != offset + width)
		परत असत्य;
	width += chunk.width;
	परत सत्य;
}


तार्किक DriveChunkMultiple::can_append(DriveBitMultiple स्थिरांक &bit) स्थिरांक
{
	जर (bit.multiple().size() != multiple_.size())
		परत असत्य;

	पूर्णांक const_drivers = 0;
	साठी (DriveChunk स्थिरांक &single : multiple_)
		जर (single.is_constant())
			const_drivers += 1;

	जर (const_drivers > 1)
		परत असत्य;

	साठी (DriveBit स्थिरांक &single : bit.multiple())
		जर (single.is_constant())
			const_drivers -= 1;

	जर (const_drivers != 0)
		परत असत्य;

	साठी (DriveChunk स्थिरांक &single : multiple_)
	{
		निवड (single.type())
		{
			स्थिती DriveType::CONSTANT: {
			} थांबा;
			स्थिती DriveType::WIRE: {
				स्वतः स्थिरांक &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				जर (!bit.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			स्थिती DriveType::PORT: {
				स्वतः स्थिरांक &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				जर (!bit.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			स्थिती DriveType::MARKER: {
				स्वतः स्थिरांक &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				जर (!bit.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			पूर्वनिर्धारित:
				परत असत्य;
		}
	}

	परत सत्य;
}

तार्किक DriveChunkMultiple::can_append(DriveChunkMultiple स्थिरांक &chunk) स्थिरांक
{
	जर (chunk.multiple().size() != multiple_.size())
		परत असत्य;

	पूर्णांक const_drivers = 0;
	साठी (DriveChunk स्थिरांक &single : multiple_)
		जर (single.is_constant())
			const_drivers += 1;

	जर (const_drivers > 1)
		परत असत्य;

	साठी (DriveChunk स्थिरांक &single : chunk.multiple())
		जर (single.is_constant())
			const_drivers -= 1;

	जर (const_drivers != 0)
		परत असत्य;

	साठी (DriveChunk स्थिरांक &single : multiple_)
	{
		निवड (single.type())
		{
			स्थिती DriveType::CONSTANT: {
			} थांबा;
			स्थिती DriveType::WIRE: {
				स्वतः स्थिरांक &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				जर (!chunk.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			स्थिती DriveType::PORT: {
				स्वतः स्थिरांक &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				जर (!chunk.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			स्थिती DriveType::MARKER: {
				स्वतः स्थिरांक &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				जर (!chunk.multiple().मोजणी(next))
					परत असत्य;
			} थांबा;
			पूर्वनिर्धारित:
				परत असत्य;
		}
	}

	परत सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveBitMultiple स्थिरांक &bit)
{
	जर (!can_append(bit))
		परत असत्य;
	width_ += 1;
	State constant;

	साठी (DriveBit स्थिरांक &single : bit.multiple())
		जर (single.is_constant())
			constant = single.constant();

	साठी (DriveChunk &single : multiple_)
	{
		निवड (single.type())
		{
			स्थिती DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} थांबा;
			स्थिती DriveType::WIRE: {
				single.wire().width += 1;
			} थांबा;
			स्थिती DriveType::PORT: {
				single.port().width += 1;
			} थांबा;
			स्थिती DriveType::MARKER: {
				single.marker().width += 1;
			} थांबा;
			पूर्वनिर्धारित:
				log_abort();
		}
	}
	परत सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveChunkMultiple स्थिरांक &chunk)
{
	जर (!can_append(chunk))
		परत असत्य;
	पूर्णांक width = chunk.size();
	width_ += width;
	Const constant;

	साठी (DriveChunk स्थिरांक &single : chunk.multiple())
		जर (single.is_constant())
			constant = single.constant();

	साठी (DriveChunk &single : multiple_)
	{
		निवड (single.type())
		{
			स्थिती DriveType::CONSTANT: {
				single.constant().append(constant);
			} थांबा;
			स्थिती DriveType::WIRE: {
				single.wire().width += width;
			} थांबा;
			स्थिती DriveType::PORT: {
				single.port().width += width;
			} थांबा;
			स्थिती DriveType::MARKER: {
				single.marker().width += width;
			} थांबा;
			पूर्वनिर्धारित:
				log_abort();
		}
	}
	परत सत्य;
}

तार्किक DriveChunk::can_append(DriveBit स्थिरांक &bit) स्थिरांक
{
	जर (size() == 0)
		परत सत्य;
	जर (bit.type() != type_)
		परत असत्य;
	निवड (type_)
	{
		स्थिती DriveType::NONE:
			परत सत्य;
		स्थिती DriveType::CONSTANT:
			परत सत्य;
		स्थिती DriveType::WIRE:
			परत wire_.can_append(bit.wire());
		स्थिती DriveType::PORT:
			परत port_.can_append(bit.port());
		स्थिती DriveType::MULTIPLE:
			परत multiple_.can_append(bit.multiple());
		पूर्वनिर्धारित:
			log_abort();
	}
}

तार्किक DriveChunk::try_append(DriveBit स्थिरांक &bit)
{
	जर (size() == 0)
		*हे = bit;
	जर (bit.type() != type_)
		परत असत्य;
	निवड (type_)
	{
		स्थिती DriveType::NONE:
			none_ += 1;
			परत सत्य;
		स्थिती DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			परत सत्य;
		स्थिती DriveType::WIRE:
			परत wire_.try_append(bit.wire());
		स्थिती DriveType::PORT:
			परत port_.try_append(bit.port());
		स्थिती DriveType::MULTIPLE:
			परत multiple_.try_append(bit.multiple());
		पूर्वनिर्धारित:
			log_abort();
	}
}


तार्किक DriveChunk::try_append(DriveChunk स्थिरांक &chunk)
{
	जर (size() == 0)
		*हे = chunk;
	जर (chunk.type_ != type_)
		परत असत्य;
	निवड (type_)
	{
		स्थिती DriveType::NONE:
			none_ += chunk.none_;
			परत सत्य;
		स्थिती DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			परत सत्य;
		स्थिती DriveType::WIRE:
			परत wire_.try_append(chunk.wire());
		स्थिती DriveType::PORT:
			परत port_.try_append(chunk.port());
		स्थिती DriveType::MARKER:
			परत marker_.try_append(chunk.marker());
		स्थिती DriveType::MULTIPLE:
			परत multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

शून्य DriveSpec::append(DriveBit स्थिरांक &bit)
{
	hash_ = 0;
	जर (!packed()) {
		bits_.ढकला_back(bit);
		width_ += 1;
		परत;
	}

	जर (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

शून्य DriveSpec::append(DriveChunk स्थिरांक &chunk)
{
	hash_ = 0;
	pack();
	जर (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

शून्य DriveSpec::pack() स्थिरांक {
	जर (bits_.empty())
		परत;
	std::vector<DriveBit> bits(std::move(bits_));
	साठी (स्वतः &bit : bits)
		जर (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

शून्य DriveSpec::unpack() स्थिरांक {
	जर (chunks_.empty())
		परत;
	साठी (स्वतः &chunk : chunks_)
	{
		साठी (पूर्णांक i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

शून्य DriveSpec::compute_width()
{
	width_ = 0;
	साठी (स्वतः स्थिरांक &chunk : chunks_)
		width_ += chunk.size();
}

शून्य DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	जर (first_edges.emplace(src, dst).first->second == dst)
		परत;
	जर (second_edges.emplace(src, dst).first->second == dst)
		परत;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::काढा_edge(DriveBitId src)
{
	// TODO unused I think?
	स्वतः found_more = more_edges.find(src);
	जर (found_more != more_edges.end()) {
		स्वतः result = found_more->second.काढा();
		जर (found_more->second.empty())
			more_edges.erase(found_more);
		परत result;
	}

	स्वतः found_second = second_edges.find(src);
	जर (found_second != second_edges.end()) {
		स्वतः result = found_second->second;
		second_edges.erase(found_second);
		परत result;
	}

	स्वतः found_first = first_edges.find(src);
	जर (found_first != first_edges.end()) {
		स्वतः result = found_first->second;
		first_edges.erase(found_first);
		परत result;
	}

	परत DriveBitId();
}

शून्य DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

तार्किक DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	परत first_edges.मोजणी(src);
}

पूर्णांक DriverMap::DriveBitGraph::मोजणी(DriveBitId src)
{
	जर (!first_edges.मोजणी(src))
		परत 0;
	जर (!second_edges.मोजणी(src))
		परत 1;
	स्वतः found = more_edges.find(src);
	जर (found == more_edges.end())
		परत 2;
	परत GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णांक index)
{
	जर (index == 0)
		परत first_edges.at(src);
	अन्यथा जर (index == 1)
		परत second_edges.at(src);
	अन्यथा
		परत *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit स्थिरांक &bit)
{
	निवड (bit.type())
	{
		स्थिती DriveType::NONE:
			परत BitMode::NONE;
		स्थिती DriveType::CONSTANT:
			// TODO how to handle Sx here?
			परत bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		स्थिती DriveType::WIRE: {
			स्वतः स्थिरांक &wire = bit.wire();
			तार्किक driver = wire.wire->port_आदान;
			तार्किक driven = wire.wire->port_output;

			जर (driver && !driven)
				परत BitMode::DRIVER;
			अन्यथा जर (driven && !driver)
				परत BitMode::DRIVEN;
			अन्यथा जर (driver && driven)
				परत BitMode::TRISTATE;
			अन्यथा
				परत keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		स्थिती DriveType::PORT: {
			स्वतः स्थिरांक &port = bit.port();
			तार्किक driver = celltypes.cell_output(port.cell->type, port.port);
			तार्किक driven = celltypes.cell_आदान(port.cell->type, port.port);
			जर (driver && !driven)
				परत BitMode::DRIVER;
			अन्यथा जर (driven && !driver)
				परत BitMode::DRIVEN_UNIQUE;
			अन्यथा
				परत BitMode::TRISTATE;
		}
		स्थिती DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		पूर्वनिर्धारित:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit स्थिरांक &bit)
{
	निवड (bit.type())
	{
		स्थिती DriveType::NONE:
			परत -1;
		स्थिती DriveType::CONSTANT:
			परत (पूर्णांक)bit.constant();
		स्थिती DriveType::WIRE: {
			स्वतः स्थिरांक &wire_bit = bit.wire();
			पूर्णांक offset = next_offset;
			स्वतः insertion = wire_offsets.emplace(wire_bit.wire, offset);
			जर (insertion.second) {
				जर (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			परत insertion.first->second.id + wire_bit.offset;
		}
		स्थिती DriveType::PORT: {
			स्वतः स्थिरांक &port_bit = bit.port();
			स्वतः किल्ली = std::make_pair(port_bit.cell, port_bit.port);
			पूर्णांक offset = next_offset;
			स्वतः insertion = port_offsets.emplace(किल्ली, offset);
			जर (insertion.second) {
				पूर्णांक width = port_bit.cell->connections().at(port_bit.port).size();
				जर (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			परत insertion.first->second.id + port_bit.offset;
		}
		पूर्वनिर्धारित:
			log_assert(असत्य && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	स्वतः found_isolated = isolated_drive_bits.find(id);
	जर (found_isolated != isolated_drive_bits.end())
		परत found_isolated->second;

	स्वतः found = drive_bits.upper_bound(id);
	जर (found == drive_bits.begin()) {
		परत id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	जर (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} अन्यथा {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	परत result;
}

शून्य DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	जर (driven_id == driver_id)
		परत;

	same_driver.merge(driven_id, driver_id);

	साठी (पूर्णांक i = 0, end = connected_drivers.मोजणी(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	साठी (पूर्णांक i = 0, end = connected_undirected.मोजणी(driven_id); i != end; ++i)
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
	साठी (स्वतः स्थिरांक &conn : module->connections())
		add(conn.first, conn.second);

	साठी (स्वतः cell : module->cells())
		साठी (स्वतः स्थिरांक &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
शून्य DriverMap::add(DriveBit स्थिरांक &a, DriveBit स्थिरांक &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	जर (a_id == b_id)
		परत;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	जर (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	अन्यथा जर (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	अन्यथा जर (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	अन्यथा जर (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	अन्यथा जर (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	अन्यथा जर (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	अन्यथा
		connect_undirected(a_id, b_id);

	परत;
}

// Specialized version that avoids unpacking
शून्य DriverMap::add(SigSpec स्थिरांक &a, SigSpec स्थिरांक &b)
{
	log_assert(a.size() == b.size());
	स्वतः स्थिरांक &a_chunks = a.chunks();
	स्वतः स्थिरांक &b_chunks = b.chunks();

	स्वतः a_chunk = a_chunks.begin();
	स्वतः a_end = a_chunks.end();
	पूर्णांक a_offset = 0;

	स्वतः b_chunk = b_chunks.begin();
	पूर्णांक b_offset = 0;

	SigChunk tmp_a, tmp_b;
	पर्यंत (a_chunk != a_end) {
		पूर्णांक a_width = a_chunk->width - a_offset;
		जर (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			सुरूठेवा;
		}
		पूर्णांक b_width = b_chunk->width - b_offset;
		जर (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			सुरूठेवा;
		}
		पूर्णांक width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk स्थिरांक &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk स्थिरांक &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

शून्य DriverMap::add_port(Cell *cell, IdString port, SigSpec स्थिरांक &b)
{
	पूर्णांक offset = 0;
	साठी (स्वतः स्थिरांक &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

शून्य DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, पूर्णांक> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	साठी (पूर्णांक pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		जर (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		जर (connected_drivers.contains(current))
			drivers.emplace(current);

		पूर्णांक undirected_driver_मोजणी = connected_undirected.मोजणी(current);

		साठी (पूर्णांक i = 0; i != undirected_driver_मोजणी; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	जर (drivers.empty())
		साठी (स्वतः seen_id : seen)
			drivers.emplace(seen_id);

	साठी (स्वतः driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		साठी (पूर्णांक pos = 0; pos < GetSize(distance); ++pos) {
			स्वतः current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णांक undirected_driver_मोजणी = connected_undirected.मोजणी(current);

			साठी (पूर्णांक i = 0; i != undirected_driver_मोजणी; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				स्वतः emplaced = distance.emplace(next, current_it->second + 1);
				जर (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	साठी (स्वतः seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::चालक()(DriveBit स्थिरांक &bit)
{
	जर (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		परत bit;
	जर (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		साठी (स्वतः स्थिरांक &inner : bit.multiple().multiple())
			result.merge((*हे)(inner));
		परत result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	जर (mode == BitMode::KEEP && bit_repr_id != bit_id)
		परत bit_repr;

	पूर्णांक implicit_driver_मोजणी = connected_drivers.मोजणी(bit_repr_id);
	जर (connected_undirected.contains(bit_repr_id) && !oriented_present.मोजणी(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	जर (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	साठी (पूर्णांक i = 0; i != implicit_driver_मोजणी; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णांक oriented_driver_मोजणी = connected_oriented.मोजणी(bit_repr_id);
	साठी (पूर्णांक i = 0; i != oriented_driver_मोजणी; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	परत driver;
}

DriveSpec DriverMap::चालक()(DriveSpec spec)
{
	DriveSpec result;

	साठी (पूर्णांक i = 0, width = spec.size(); i != width; ++i)
		result.append((*हे)(spec[i]));

	परत result;
}

std::string log_signal(DriveChunkWire स्थिरांक &chunk)
{
	std::string id = chunk.wire->name.unescape();
	जर (chunk.is_whole())
		परत id;
	जर (chunk.width == 1)
		परत stringf("%s [%d]", id, chunk.offset);
	परत stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort स्थिरांक &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	जर (chunk.is_whole())
		परत stringf("%s <%s>", cell_id, port_id);
	जर (chunk.width == 1)
		परत stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	परत stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker स्थिरांक &chunk)
{
	जर (chunk.width == 1)
		परत stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	परत stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk स्थिरांक &chunk)
{
	निवड (chunk.type())
	{
		स्थिती DriveType::NONE:
			परत stringf("<none x%d>", chunk.size());
		स्थिती DriveType::CONSTANT:
			परत log_const(chunk.constant());
		स्थिती DriveType::WIRE:
			परत log_signal(chunk.wire());
		स्थिती DriveType::PORT:
			परत log_signal(chunk.port());
		स्थिती DriveType::MARKER:
			परत log_signal(chunk.marker());
		स्थिती DriveType::MULTIPLE: {
			std::string str = "<multiple";
			स्थिरांक अक्षर *sep = " ";
			साठी (स्वतः स्थिरांक &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			परत str;
		}
		पूर्वनिर्धारित:
			log_abort();
	}
}

std::string log_signal(DriveSpec स्थिरांक &spec)
{
	स्वतः &chunks = spec.chunks();
	जर (chunks.empty())
		परत "{}";
	जर (chunks.size() == 1)
		परत log_signal(chunks[0]);

	std::string str;
	स्थिरांक अक्षर *sep = "{ ";

	साठी (स्वतः i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	परत str;
}

YOSYS_NAMESPACE_END
