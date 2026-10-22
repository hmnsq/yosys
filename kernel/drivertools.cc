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

DriveBit::DriveBit(SigBit ध्रुव &bit)
{
	यदि (bit.is_wire())
		*एतत् = DriveBitWire(bit.wire, bit.offset);
	अन्यथा
		*एतत् = bit.data;
}

शून्य DriveBit::merge(DriveBit ध्रुव &other)
{
	यदि (other.type_ == DriveType::NONE)
		प्रत्यागम;
	यदि (type_ == DriveType::NONE) {
		*एतत् = other;
		प्रत्यागम;
	}
	यदि (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*एतत्));
		*एतत् = std::move(multi);
	}
	multiple().merge(other);
}


शून्य DriveBitMultiple::merge(DriveBit ध्रुव &single)
{
	यदि (single.type() == DriveType::NONE)
		प्रत्यागम;
	यदि (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		प्रत्यागम;
	}
	multiple_.emplace(single);
}

शून्य DriveBitMultiple::merge(DriveBit &&single)
{
	यदि (single.type() == DriveType::NONE)
		प्रत्यागम;
	यदि (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		प्रत्यागम;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::चालक[](पूर्णाङ्क i) ध्रुव
{
	DriveBitMultiple result;
	प्रति (स्वतः ध्रुव &single : multiple_)
		result.merge(single[i]);
	प्रत्यागम result;
}


तार्किक DriveChunkWire::can_append(DriveBitWire ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.wire == wire && bit.offset == offset + width;
}

तार्किक DriveChunkWire::try_append(DriveBitWire ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	width += 1;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkWire::try_append(DriveChunkWire ध्रुव &chunk)
{
	यदि (chunk.wire != wire || chunk.offset != offset + width)
		प्रत्यागम असत्य;
	width += chunk.width;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkPort::can_append(DriveBitPort ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

तार्किक DriveChunkPort::try_append(DriveBitPort ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	width += 1;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkPort::try_append(DriveChunkPort ध्रुव &chunk)
{
	यदि (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		प्रत्यागम असत्य;
	width += chunk.width;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkMarker::can_append(DriveBitMarker ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.marker == marker && bit.offset == offset + width;
}

तार्किक DriveChunkMarker::try_append(DriveBitMarker ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	width += 1;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkMarker::try_append(DriveChunkMarker ध्रुव &chunk)
{
	यदि (chunk.marker != marker || chunk.offset != offset + width)
		प्रत्यागम असत्य;
	width += chunk.width;
	प्रत्यागम सत्य;
}


तार्किक DriveChunkMultiple::can_append(DriveBitMultiple ध्रुव &bit) ध्रुव
{
	यदि (bit.multiple().size() != multiple_.size())
		प्रत्यागम असत्य;

	पूर्णाङ्क const_drivers = 0;
	प्रति (DriveChunk ध्रुव &single : multiple_)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		प्रत्यागम असत्य;

	प्रति (DriveBit ध्रुव &single : bit.multiple())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				स्वतः ध्रुव &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				यदि (!bit.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				स्वतः ध्रुव &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				यदि (!bit.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				स्वतः ध्रुव &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				यदि (!bit.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			स्वभाव:
				प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

तार्किक DriveChunkMultiple::can_append(DriveChunkMultiple ध्रुव &chunk) ध्रुव
{
	यदि (chunk.multiple().size() != multiple_.size())
		प्रत्यागम असत्य;

	पूर्णाङ्क const_drivers = 0;
	प्रति (DriveChunk ध्रुव &single : multiple_)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : chunk.multiple())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				स्वतः ध्रुव &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				यदि (!chunk.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				स्वतः ध्रुव &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				यदि (!chunk.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				स्वतः ध्रुव &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				यदि (!chunk.multiple().गिनती(next))
					प्रत्यागम असत्य;
			} भङ्ग;
			स्वभाव:
				प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveBitMultiple ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	width_ += 1;
	State constant;

	प्रति (DriveBit ध्रुव &single : bit.multiple())
		यदि (single.is_constant())
			constant = single.constant();

	प्रति (DriveChunk &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				single.wire().width += 1;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				single.port().width += 1;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				single.marker().width += 1;
			} भङ्ग;
			स्वभाव:
				log_abort();
		}
	}
	प्रत्यागम सत्य;
}

तार्किक DriveChunkMultiple::try_append(DriveChunkMultiple ध्रुव &chunk)
{
	यदि (!can_append(chunk))
		प्रत्यागम असत्य;
	पूर्णाङ्क width = chunk.size();
	width_ += width;
	Const constant;

	प्रति (DriveChunk ध्रुव &single : chunk.multiple())
		यदि (single.is_constant())
			constant = single.constant();

	प्रति (DriveChunk &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(constant);
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				single.wire().width += width;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				single.port().width += width;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				single.marker().width += width;
			} भङ्ग;
			स्वभाव:
				log_abort();
		}
	}
	प्रत्यागम सत्य;
}

तार्किक DriveChunk::can_append(DriveBit ध्रुव &bit) ध्रुव
{
	यदि (size() == 0)
		प्रत्यागम सत्य;
	यदि (bit.type() != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.can_append(bit.wire());
		पक्ष DriveType::PORT:
			प्रत्यागम port_.can_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			प्रत्यागम multiple_.can_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}

तार्किक DriveChunk::try_append(DriveBit ध्रुव &bit)
{
	यदि (size() == 0)
		*एतत् = bit;
	यदि (bit.type() != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += 1;
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.try_append(bit.wire());
		पक्ष DriveType::PORT:
			प्रत्यागम port_.try_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			प्रत्यागम multiple_.try_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}


तार्किक DriveChunk::try_append(DriveChunk ध्रुव &chunk)
{
	यदि (size() == 0)
		*एतत् = chunk;
	यदि (chunk.type_ != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += chunk.none_;
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.try_append(chunk.wire());
		पक्ष DriveType::PORT:
			प्रत्यागम port_.try_append(chunk.port());
		पक्ष DriveType::MARKER:
			प्रत्यागम marker_.try_append(chunk.marker());
		पक्ष DriveType::MULTIPLE:
			प्रत्यागम multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

शून्य DriveSpec::append(DriveBit ध्रुव &bit)
{
	hash_ = 0;
	यदि (!packed()) {
		bits_.धकेलो_back(bit);
		width_ += 1;
		प्रत्यागम;
	}

	यदि (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

शून्य DriveSpec::append(DriveChunk ध्रुव &chunk)
{
	hash_ = 0;
	pack();
	यदि (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

शून्य DriveSpec::pack() ध्रुव {
	यदि (bits_.empty())
		प्रत्यागम;
	std::vector<DriveBit> bits(std::move(bits_));
	प्रति (स्वतः &bit : bits)
		यदि (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

शून्य DriveSpec::unpack() ध्रुव {
	यदि (chunks_.empty())
		प्रत्यागम;
	प्रति (स्वतः &chunk : chunks_)
	{
		प्रति (पूर्णाङ्क i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

शून्य DriveSpec::compute_width()
{
	width_ = 0;
	प्रति (स्वतः ध्रुव &chunk : chunks_)
		width_ += chunk.size();
}

शून्य DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	यदि (first_edges.emplace(src, dst).first->second == dst)
		प्रत्यागम;
	यदि (second_edges.emplace(src, dst).first->second == dst)
		प्रत्यागम;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::निकालो_edge(DriveBitId src)
{
	// TODO unused I think?
	स्वतः found_more = more_edges.find(src);
	यदि (found_more != more_edges.end()) {
		स्वतः result = found_more->second.निकालो();
		यदि (found_more->second.empty())
			more_edges.erase(found_more);
		प्रत्यागम result;
	}

	स्वतः found_second = second_edges.find(src);
	यदि (found_second != second_edges.end()) {
		स्वतः result = found_second->second;
		second_edges.erase(found_second);
		प्रत्यागम result;
	}

	स्वतः found_first = first_edges.find(src);
	यदि (found_first != first_edges.end()) {
		स्वतः result = found_first->second;
		first_edges.erase(found_first);
		प्रत्यागम result;
	}

	प्रत्यागम DriveBitId();
}

शून्य DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

तार्किक DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	प्रत्यागम first_edges.गिनती(src);
}

पूर्णाङ्क DriverMap::DriveBitGraph::गिनती(DriveBitId src)
{
	यदि (!first_edges.गिनती(src))
		प्रत्यागम 0;
	यदि (!second_edges.गिनती(src))
		प्रत्यागम 1;
	स्वतः found = more_edges.find(src);
	यदि (found == more_edges.end())
		प्रत्यागम 2;
	प्रत्यागम GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णाङ्क index)
{
	यदि (index == 0)
		प्रत्यागम first_edges.at(src);
	अन्यथा यदि (index == 1)
		प्रत्यागम second_edges.at(src);
	अन्यथा
		प्रत्यागम *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ध्रुव &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम BitMode::NONE;
		पक्ष DriveType::CONSTANT:
			// TODO how to handle Sx here?
			प्रत्यागम bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		पक्ष DriveType::WIRE: {
			स्वतः ध्रुव &wire = bit.wire();
			तार्किक driver = wire.wire->port_निवेश;
			तार्किक driven = wire.wire->port_output;

			यदि (driver && !driven)
				प्रत्यागम BitMode::DRIVER;
			अन्यथा यदि (driven && !driver)
				प्रत्यागम BitMode::DRIVEN;
			अन्यथा यदि (driver && driven)
				प्रत्यागम BitMode::TRISTATE;
			अन्यथा
				प्रत्यागम keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		पक्ष DriveType::PORT: {
			स्वतः ध्रुव &port = bit.port();
			तार्किक driver = celltypes.cell_output(port.cell->type, port.port);
			तार्किक driven = celltypes.cell_निवेश(port.cell->type, port.port);
			यदि (driver && !driven)
				प्रत्यागम BitMode::DRIVER;
			अन्यथा यदि (driven && !driver)
				प्रत्यागम BitMode::DRIVEN_UNIQUE;
			अन्यथा
				प्रत्यागम BitMode::TRISTATE;
		}
		पक्ष DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		स्वभाव:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ध्रुव &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम -1;
		पक्ष DriveType::CONSTANT:
			प्रत्यागम (पूर्णाङ्क)bit.constant();
		पक्ष DriveType::WIRE: {
			स्वतः ध्रुव &wire_bit = bit.wire();
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
			प्रत्यागम insertion.first->second.id + wire_bit.offset;
		}
		पक्ष DriveType::PORT: {
			स्वतः ध्रुव &port_bit = bit.port();
			स्वतः कुंजी = std::make_pair(port_bit.cell, port_bit.port);
			पूर्णाङ्क offset = next_offset;
			स्वतः insertion = port_offsets.emplace(कुंजी, offset);
			यदि (insertion.second) {
				पूर्णाङ्क width = port_bit.cell->connections().at(port_bit.port).size();
				यदि (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			प्रत्यागम insertion.first->second.id + port_bit.offset;
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
		प्रत्यागम found_isolated->second;

	स्वतः found = drive_bits.upper_bound(id);
	यदि (found == drive_bits.begin()) {
		प्रत्यागम id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	यदि (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} अन्यथा {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	प्रत्यागम result;
}

शून्य DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	यदि (driven_id == driver_id)
		प्रत्यागम;

	same_driver.merge(driven_id, driver_id);

	प्रति (पूर्णाङ्क i = 0, end = connected_drivers.गिनती(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	प्रति (पूर्णाङ्क i = 0, end = connected_undirected.गिनती(driven_id); i != end; ++i)
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
	प्रति (स्वतः ध्रुव &conn : module->connections())
		add(conn.first, conn.second);

	प्रति (स्वतः cell : module->cells())
		प्रति (स्वतः ध्रुव &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
शून्य DriverMap::add(DriveBit ध्रुव &a, DriveBit ध्रुव &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	यदि (a_id == b_id)
		प्रत्यागम;

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

	प्रत्यागम;
}

// Specialized version that avoids unpacking
शून्य DriverMap::add(SigSpec ध्रुव &a, SigSpec ध्रुव &b)
{
	log_assert(a.size() == b.size());
	स्वतः ध्रुव &a_chunks = a.chunks();
	स्वतः ध्रुव &b_chunks = b.chunks();

	स्वतः a_chunk = a_chunks.begin();
	स्वतः a_end = a_chunks.end();
	पूर्णाङ्क a_offset = 0;

	स्वतः b_chunk = b_chunks.begin();
	पूर्णाङ्क b_offset = 0;

	SigChunk tmp_a, tmp_b;
	यावत् (a_chunk != a_end) {
		पूर्णाङ्क a_width = a_chunk->width - a_offset;
		यदि (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			अनुवर्त;
		}
		पूर्णाङ्क b_width = b_chunk->width - b_offset;
		यदि (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			अनुवर्त;
		}
		पूर्णाङ्क width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ध्रुव &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ध्रुव &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

शून्य DriverMap::add_port(Cell *cell, IdString port, SigSpec ध्रुव &b)
{
	पूर्णाङ्क offset = 0;
	प्रति (स्वतः ध्रुव &chunk : b.chunks()) {
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

	प्रति (पूर्णाङ्क pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		यदि (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		यदि (connected_drivers.contains(current))
			drivers.emplace(current);

		पूर्णाङ्क undirected_driver_गिनती = connected_undirected.गिनती(current);

		प्रति (पूर्णाङ्क i = 0; i != undirected_driver_गिनती; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	यदि (drivers.empty())
		प्रति (स्वतः seen_id : seen)
			drivers.emplace(seen_id);

	प्रति (स्वतः driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		प्रति (पूर्णाङ्क pos = 0; pos < GetSize(distance); ++pos) {
			स्वतः current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णाङ्क undirected_driver_गिनती = connected_undirected.गिनती(current);

			प्रति (पूर्णाङ्क i = 0; i != undirected_driver_गिनती; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				स्वतः emplaced = distance.emplace(next, current_it->second + 1);
				यदि (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	प्रति (स्वतः seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::चालक()(DriveBit ध्रुव &bit)
{
	यदि (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		प्रत्यागम bit;
	यदि (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		प्रति (स्वतः ध्रुव &inner : bit.multiple().multiple())
			result.merge((*एतत्)(inner));
		प्रत्यागम result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	यदि (mode == BitMode::KEEP && bit_repr_id != bit_id)
		प्रत्यागम bit_repr;

	पूर्णाङ्क implicit_driver_गिनती = connected_drivers.गिनती(bit_repr_id);
	यदि (connected_undirected.contains(bit_repr_id) && !oriented_present.गिनती(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	यदि (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	प्रति (पूर्णाङ्क i = 0; i != implicit_driver_गिनती; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णाङ्क oriented_driver_गिनती = connected_oriented.गिनती(bit_repr_id);
	प्रति (पूर्णाङ्क i = 0; i != oriented_driver_गिनती; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	प्रत्यागम driver;
}

DriveSpec DriverMap::चालक()(DriveSpec spec)
{
	DriveSpec result;

	प्रति (पूर्णाङ्क i = 0, width = spec.size(); i != width; ++i)
		result.append((*एतत्)(spec[i]));

	प्रत्यागम result;
}

std::string log_signal(DriveChunkWire ध्रुव &chunk)
{
	std::string id = chunk.wire->name.unescape();
	यदि (chunk.is_whole())
		प्रत्यागम id;
	यदि (chunk.width == 1)
		प्रत्यागम stringf("%s [%d]", id, chunk.offset);
	प्रत्यागम stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ध्रुव &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	यदि (chunk.is_whole())
		प्रत्यागम stringf("%s <%s>", cell_id, port_id);
	यदि (chunk.width == 1)
		प्रत्यागम stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	प्रत्यागम stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ध्रुव &chunk)
{
	यदि (chunk.width == 1)
		प्रत्यागम stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	प्रत्यागम stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ध्रुव &chunk)
{
	विकल्प (chunk.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम stringf("<none x%d>", chunk.size());
		पक्ष DriveType::CONSTANT:
			प्रत्यागम log_const(chunk.constant());
		पक्ष DriveType::WIRE:
			प्रत्यागम log_signal(chunk.wire());
		पक्ष DriveType::PORT:
			प्रत्यागम log_signal(chunk.port());
		पक्ष DriveType::MARKER:
			प्रत्यागम log_signal(chunk.marker());
		पक्ष DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ध्रुव वर्ण *sep = " ";
			प्रति (स्वतः ध्रुव &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			प्रत्यागम str;
		}
		स्वभाव:
			log_abort();
	}
}

std::string log_signal(DriveSpec ध्रुव &spec)
{
	स्वतः &chunks = spec.chunks();
	यदि (chunks.empty())
		प्रत्यागम "{}";
	यदि (chunks.size() == 1)
		प्रत्यागम log_signal(chunks[0]);

	std::string str;
	ध्रुव वर्ण *sep = "{ ";

	प्रति (स्वतः i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	प्रत्यागम str;
}

YOSYS_NAMESPACE_END
