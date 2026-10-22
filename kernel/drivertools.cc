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
	सचे (bit.is_wire())
		*एतत् = DriveBitWire(bit.wire, bit.offset);
	अञ्ञथा
		*एतत् = bit.data;
}

शून्य DriveBit::merge(DriveBit ध्रुव &other)
{
	सचे (other.type_ == DriveType::NONE)
		पटिनिवत्त;
	सचे (type_ == DriveType::NONE) {
		*एतत् = other;
		पटिनिवत्त;
	}
	सचे (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*एतत्));
		*एतत् = std::move(multi);
	}
	multiple().merge(other);
}


शून्य DriveBitMultiple::merge(DriveBit ध्रुव &single)
{
	सचे (single.type() == DriveType::NONE)
		पटिनिवत्त;
	सचे (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		पटिनिवत्त;
	}
	multiple_.emplace(single);
}

शून्य DriveBitMultiple::merge(DriveBit &&single)
{
	सचे (single.type() == DriveType::NONE)
		पटिनिवत्त;
	सचे (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		पटिनिवत्त;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](पूर्णाङ्क i) ध्रुव
{
	DriveBitMultiple result;
	अत्थाय (auto ध्रुव &single : multiple_)
		result.merge(single[i]);
	पटिनिवत्त result;
}


तार्किक DriveChunkWire::can_append(DriveBitWire ध्रुव &bit) ध्रुव
{
	पटिनिवत्त bit.wire == wire && bit.offset == offset + width;
}

तार्किक DriveChunkWire::try_append(DriveBitWire ध्रुव &bit)
{
	सचे (!can_append(bit))
		पटिनिवत्त मिच्छा;
	width += 1;
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkWire::try_append(DriveChunkWire ध्रुव &chunk)
{
	सचे (chunk.wire != wire || chunk.offset != offset + width)
		पटिनिवत्त मिच्छा;
	width += chunk.width;
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkPort::can_append(DriveBitPort ध्रुव &bit) ध्रुव
{
	पटिनिवत्त bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

तार्किक DriveChunkPort::try_append(DriveBitPort ध्रुव &bit)
{
	सचे (!can_append(bit))
		पटिनिवत्त मिच्छा;
	width += 1;
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkPort::try_append(DriveChunkPort ध्रुव &chunk)
{
	सचे (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		पटिनिवत्त मिच्छा;
	width += chunk.width;
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkMarker::can_append(DriveBitMarker ध्रुव &bit) ध्रुव
{
	पटिनिवत्त bit.marker == marker && bit.offset == offset + width;
}

तार्किक DriveChunkMarker::try_append(DriveBitMarker ध्रुव &bit)
{
	सचे (!can_append(bit))
		पटिनिवत्त मिच्छा;
	width += 1;
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkMarker::try_append(DriveChunkMarker ध्रुव &chunk)
{
	सचे (chunk.marker != marker || chunk.offset != offset + width)
		पटिनिवत्त मिच्छा;
	width += chunk.width;
	पटिनिवत्त सच्च;
}


तार्किक DriveChunkMultiple::can_append(DriveBitMultiple ध्रुव &bit) ध्रुव
{
	सचे (bit.multiple().size() != multiple_.size())
		पटिनिवत्त मिच्छा;

	पूर्णाङ्क const_drivers = 0;
	अत्थाय (DriveChunk ध्रुव &single : multiple_)
		सचे (single.is_constant())
			const_drivers += 1;

	सचे (const_drivers > 1)
		पटिनिवत्त मिच्छा;

	अत्थाय (DriveBit ध्रुव &single : bit.multiple())
		सचे (single.is_constant())
			const_drivers -= 1;

	सचे (const_drivers != 0)
		पटिनिवत्त मिच्छा;

	अत्थाय (DriveChunk ध्रुव &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				auto ध्रुव &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				सचे (!bit.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				auto ध्रुव &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				सचे (!bit.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				auto ध्रुव &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				सचे (!bit.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			स्वभाव:
				पटिनिवत्त मिच्छा;
		}
	}

	पटिनिवत्त सच्च;
}

तार्किक DriveChunkMultiple::can_append(DriveChunkMultiple ध्रुव &chunk) ध्रुव
{
	सचे (chunk.multiple().size() != multiple_.size())
		पटिनिवत्त मिच्छा;

	पूर्णाङ्क const_drivers = 0;
	अत्थाय (DriveChunk ध्रुव &single : multiple_)
		सचे (single.is_constant())
			const_drivers += 1;

	सचे (const_drivers > 1)
		पटिनिवत्त मिच्छा;

	अत्थाय (DriveChunk ध्रुव &single : chunk.multiple())
		सचे (single.is_constant())
			const_drivers -= 1;

	सचे (const_drivers != 0)
		पटिनिवत्त मिच्छा;

	अत्थाय (DriveChunk ध्रुव &single : multiple_)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				auto ध्रुव &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				सचे (!chunk.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			पक्ष DriveType::PORT: {
				auto ध्रुव &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				सचे (!chunk.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				auto ध्रुव &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				सचे (!chunk.multiple().गणनम्(next))
					पटिनिवत्त मिच्छा;
			} भङ्ग;
			स्वभाव:
				पटिनिवत्त मिच्छा;
		}
	}

	पटिनिवत्त सच्च;
}

तार्किक DriveChunkMultiple::try_append(DriveBitMultiple ध्रुव &bit)
{
	सचे (!can_append(bit))
		पटिनिवत्त मिच्छा;
	width_ += 1;
	State constant;

	अत्थाय (DriveBit ध्रुव &single : bit.multiple())
		सचे (single.is_constant())
			constant = single.constant();

	अत्थाय (DriveChunk &single : multiple_)
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
	पटिनिवत्त सच्च;
}

तार्किक DriveChunkMultiple::try_append(DriveChunkMultiple ध्रुव &chunk)
{
	सचे (!can_append(chunk))
		पटिनिवत्त मिच्छा;
	पूर्णाङ्क width = chunk.size();
	width_ += width;
	Const constant;

	अत्थाय (DriveChunk ध्रुव &single : chunk.multiple())
		सचे (single.is_constant())
			constant = single.constant();

	अत्थाय (DriveChunk &single : multiple_)
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
	पटिनिवत्त सच्च;
}

तार्किक DriveChunk::can_append(DriveBit ध्रुव &bit) ध्रुव
{
	सचे (size() == 0)
		पटिनिवत्त सच्च;
	सचे (bit.type() != type_)
		पटिनिवत्त मिच्छा;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			पटिनिवत्त सच्च;
		पक्ष DriveType::CONSTANT:
			पटिनिवत्त सच्च;
		पक्ष DriveType::WIRE:
			पटिनिवत्त wire_.can_append(bit.wire());
		पक्ष DriveType::PORT:
			पटिनिवत्त port_.can_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			पटिनिवत्त multiple_.can_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}

तार्किक DriveChunk::try_append(DriveBit ध्रुव &bit)
{
	सचे (size() == 0)
		*एतत् = bit;
	सचे (bit.type() != type_)
		पटिनिवत्त मिच्छा;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += 1;
			पटिनिवत्त सच्च;
		पक्ष DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			पटिनिवत्त सच्च;
		पक्ष DriveType::WIRE:
			पटिनिवत्त wire_.try_append(bit.wire());
		पक्ष DriveType::PORT:
			पटिनिवत्त port_.try_append(bit.port());
		पक्ष DriveType::MULTIPLE:
			पटिनिवत्त multiple_.try_append(bit.multiple());
		स्वभाव:
			log_abort();
	}
}


तार्किक DriveChunk::try_append(DriveChunk ध्रुव &chunk)
{
	सचे (size() == 0)
		*एतत् = chunk;
	सचे (chunk.type_ != type_)
		पटिनिवत्त मिच्छा;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += chunk.none_;
			पटिनिवत्त सच्च;
		पक्ष DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			पटिनिवत्त सच्च;
		पक्ष DriveType::WIRE:
			पटिनिवत्त wire_.try_append(chunk.wire());
		पक्ष DriveType::PORT:
			पटिनिवत्त port_.try_append(chunk.port());
		पक्ष DriveType::MARKER:
			पटिनिवत्त marker_.try_append(chunk.marker());
		पक्ष DriveType::MULTIPLE:
			पटिनिवत्त multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

शून्य DriveSpec::append(DriveBit ध्रुव &bit)
{
	hash_ = 0;
	सचे (!packed()) {
		bits_.प्रक्षिप_back(bit);
		width_ += 1;
		पटिनिवत्त;
	}

	सचे (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

शून्य DriveSpec::append(DriveChunk ध्रुव &chunk)
{
	hash_ = 0;
	pack();
	सचे (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

शून्य DriveSpec::pack() ध्रुव {
	सचे (bits_.empty())
		पटिनिवत्त;
	std::vector<DriveBit> bits(std::move(bits_));
	अत्थाय (auto &bit : bits)
		सचे (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

शून्य DriveSpec::unpack() ध्रुव {
	सचे (chunks_.empty())
		पटिनिवत्त;
	अत्थाय (auto &chunk : chunks_)
	{
		अत्थाय (पूर्णाङ्क i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

शून्य DriveSpec::compute_width()
{
	width_ = 0;
	अत्थाय (auto ध्रुव &chunk : chunks_)
		width_ += chunk.size();
}

शून्य DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	सचे (first_edges.emplace(src, dst).first->second == dst)
		पटिनिवत्त;
	सचे (second_edges.emplace(src, dst).first->second == dst)
		पटिनिवत्त;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::निष्कासय_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	सचे (found_more != more_edges.end()) {
		auto result = found_more->second.निष्कासय();
		सचे (found_more->second.empty())
			more_edges.erase(found_more);
		पटिनिवत्त result;
	}

	auto found_second = second_edges.find(src);
	सचे (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		पटिनिवत्त result;
	}

	auto found_first = first_edges.find(src);
	सचे (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		पटिनिवत्त result;
	}

	पटिनिवत्त DriveBitId();
}

शून्य DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

तार्किक DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	पटिनिवत्त first_edges.गणनम्(src);
}

पूर्णाङ्क DriverMap::DriveBitGraph::गणनम्(DriveBitId src)
{
	सचे (!first_edges.गणनम्(src))
		पटिनिवत्त 0;
	सचे (!second_edges.गणनम्(src))
		पटिनिवत्त 1;
	auto found = more_edges.find(src);
	सचे (found == more_edges.end())
		पटिनिवत्त 2;
	पटिनिवत्त GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णाङ्क index)
{
	सचे (index == 0)
		पटिनिवत्त first_edges.at(src);
	अञ्ञथा सचे (index == 1)
		पटिनिवत्त second_edges.at(src);
	अञ्ञथा
		पटिनिवत्त *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ध्रुव &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			पटिनिवत्त BitMode::NONE;
		पक्ष DriveType::CONSTANT:
			// TODO how to handle Sx here?
			पटिनिवत्त bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		पक्ष DriveType::WIRE: {
			auto ध्रुव &wire = bit.wire();
			तार्किक driver = wire.wire->port_निवेशः;
			तार्किक driven = wire.wire->port_output;

			सचे (driver && !driven)
				पटिनिवत्त BitMode::DRIVER;
			अञ्ञथा सचे (driven && !driver)
				पटिनिवत्त BitMode::DRIVEN;
			अञ्ञथा सचे (driver && driven)
				पटिनिवत्त BitMode::TRISTATE;
			अञ्ञथा
				पटिनिवत्त keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		पक्ष DriveType::PORT: {
			auto ध्रुव &port = bit.port();
			तार्किक driver = celltypes.cell_output(port.cell->type, port.port);
			तार्किक driven = celltypes.cell_निवेशः(port.cell->type, port.port);
			सचे (driver && !driven)
				पटिनिवत्त BitMode::DRIVER;
			अञ्ञथा सचे (driven && !driver)
				पटिनिवत्त BitMode::DRIVEN_UNIQUE;
			अञ्ञथा
				पटिनिवत्त BitMode::TRISTATE;
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
			पटिनिवत्त -1;
		पक्ष DriveType::CONSTANT:
			पटिनिवत्त (पूर्णाङ्क)bit.constant();
		पक्ष DriveType::WIRE: {
			auto ध्रुव &wire_bit = bit.wire();
			पूर्णाङ्क offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			सचे (insertion.second) {
				सचे (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अञ्ञथा
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			पटिनिवत्त insertion.first->second.id + wire_bit.offset;
		}
		पक्ष DriveType::PORT: {
			auto ध्रुव &port_bit = bit.port();
			auto कुञ्चिका = std::make_pair(port_bit.cell, port_bit.port);
			पूर्णाङ्क offset = next_offset;
			auto insertion = port_offsets.emplace(कुञ्चिका, offset);
			सचे (insertion.second) {
				पूर्णाङ्क width = port_bit.cell->connections().at(port_bit.port).size();
				सचे (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अञ्ञथा
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			पटिनिवत्त insertion.first->second.id + port_bit.offset;
		}
		स्वभाव:
			log_assert(मिच्छा && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	सचे (found_isolated != isolated_drive_bits.end())
		पटिनिवत्त found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	सचे (found == drive_bits.begin()) {
		पटिनिवत्त id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	सचे (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} अञ्ञथा {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	पटिनिवत्त result;
}

शून्य DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	सचे (driven_id == driver_id)
		पटिनिवत्त;

	same_driver.merge(driven_id, driver_id);

	अत्थाय (पूर्णाङ्क i = 0, end = connected_drivers.गणनम्(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	अत्थाय (पूर्णाङ्क i = 0, end = connected_undirected.गणनम्(driven_id); i != end; ++i)
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
	अत्थाय (auto ध्रुव &conn : module->connections())
		add(conn.first, conn.second);

	अत्थाय (auto cell : module->cells())
		अत्थाय (auto ध्रुव &conn : cell->connections())
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

	सचे (a_id == b_id)
		पटिनिवत्त;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	सचे (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	अञ्ञथा सचे (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	अञ्ञथा सचे (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	अञ्ञथा सचे (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	अञ्ञथा सचे (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	अञ्ञथा सचे (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	अञ्ञथा
		connect_undirected(a_id, b_id);

	पटिनिवत्त;
}

// Specialized version that avoids unpacking
शून्य DriverMap::add(SigSpec ध्रुव &a, SigSpec ध्रुव &b)
{
	log_assert(a.size() == b.size());
	auto ध्रुव &a_chunks = a.chunks();
	auto ध्रुव &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	पूर्णाङ्क a_offset = 0;

	auto b_chunk = b_chunks.begin();
	पूर्णाङ्क b_offset = 0;

	SigChunk tmp_a, tmp_b;
	याव (a_chunk != a_end) {
		पूर्णाङ्क a_width = a_chunk->width - a_offset;
		सचे (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			अनुवर्त;
		}
		पूर्णाङ्क b_width = b_chunk->width - b_offset;
		सचे (b_width == 0) {
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
	अत्थाय (auto ध्रुव &chunk : b.chunks()) {
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

	अत्थाय (पूर्णाङ्क pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		सचे (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		सचे (connected_drivers.contains(current))
			drivers.emplace(current);

		पूर्णाङ्क undirected_driver_गणनम् = connected_undirected.गणनम्(current);

		अत्थाय (पूर्णाङ्क i = 0; i != undirected_driver_गणनम्; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	सचे (drivers.empty())
		अत्थाय (auto seen_id : seen)
			drivers.emplace(seen_id);

	अत्थाय (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		अत्थाय (पूर्णाङ्क pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णाङ्क undirected_driver_गणनम् = connected_undirected.गणनम्(current);

			अत्थाय (पूर्णाङ्क i = 0; i != undirected_driver_गणनम्; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				सचे (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	अत्थाय (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ध्रुव &bit)
{
	सचे (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		पटिनिवत्त bit;
	सचे (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		अत्थाय (auto ध्रुव &inner : bit.multiple().multiple())
			result.merge((*एतत्)(inner));
		पटिनिवत्त result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	सचे (mode == BitMode::KEEP && bit_repr_id != bit_id)
		पटिनिवत्त bit_repr;

	पूर्णाङ्क implicit_driver_गणनम् = connected_drivers.गणनम्(bit_repr_id);
	सचे (connected_undirected.contains(bit_repr_id) && !oriented_present.गणनम्(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	सचे (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	अत्थाय (पूर्णाङ्क i = 0; i != implicit_driver_गणनम्; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णाङ्क oriented_driver_गणनम् = connected_oriented.गणनम्(bit_repr_id);
	अत्थाय (पूर्णाङ्क i = 0; i != oriented_driver_गणनम्; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	पटिनिवत्त driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	अत्थाय (पूर्णाङ्क i = 0, width = spec.size(); i != width; ++i)
		result.append((*एतत्)(spec[i]));

	पटिनिवत्त result;
}

std::string log_signal(DriveChunkWire ध्रुव &chunk)
{
	std::string id = chunk.wire->name.unescape();
	सचे (chunk.is_whole())
		पटिनिवत्त id;
	सचे (chunk.width == 1)
		पटिनिवत्त stringf("%s [%d]", id, chunk.offset);
	पटिनिवत्त stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ध्रुव &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	सचे (chunk.is_whole())
		पटिनिवत्त stringf("%s <%s>", cell_id, port_id);
	सचे (chunk.width == 1)
		पटिनिवत्त stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	पटिनिवत्त stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ध्रुव &chunk)
{
	सचे (chunk.width == 1)
		पटिनिवत्त stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	पटिनिवत्त stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ध्रुव &chunk)
{
	विकल्प (chunk.type())
	{
		पक्ष DriveType::NONE:
			पटिनिवत्त stringf("<none x%d>", chunk.size());
		पक्ष DriveType::CONSTANT:
			पटिनिवत्त log_const(chunk.constant());
		पक्ष DriveType::WIRE:
			पटिनिवत्त log_signal(chunk.wire());
		पक्ष DriveType::PORT:
			पटिनिवत्त log_signal(chunk.port());
		पक्ष DriveType::MARKER:
			पटिनिवत्त log_signal(chunk.marker());
		पक्ष DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ध्रुव वर्ण *sep = " ";
			अत्थाय (auto ध्रुव &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			पटिनिवत्त str;
		}
		स्वभाव:
			log_abort();
	}
}

std::string log_signal(DriveSpec ध्रुव &spec)
{
	auto &chunks = spec.chunks();
	सचे (chunks.empty())
		पटिनिवत्त "{}";
	सचे (chunks.size() == 1)
		पटिनिवत्त log_signal(chunks[0]);

	std::string str;
	ध्रुव वर्ण *sep = "{ ";

	अत्थाय (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	पटिनिवत्त str;
}

YOSYS_NAMESPACE_END
