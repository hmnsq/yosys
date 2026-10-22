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

#समावेश "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit स्थिर &bit)
{
	अगर (bit.is_wire())
		*यह = DriveBitWire(bit.wire, bit.offset);
	अन्यथा
		*यह = bit.तथ्य;
}

व्योम DriveBit::merge(DriveBit स्थिर &other)
{
	अगर (other.type_ == DriveType::NONE)
		वापस;
	अगर (type_ == DriveType::NONE) {
		*यह = other;
		वापस;
	}
	अगर (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*यह));
		*यह = std::move(multi);
	}
	multiple().merge(other);
}


व्योम DriveBitMultiple::merge(DriveBit स्थिर &single)
{
	अगर (single.type() == DriveType::NONE)
		वापस;
	अगर (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		वापस;
	}
	multiple_.emplace(single);
}

व्योम DriveBitMultiple::merge(DriveBit &&single)
{
	अगर (single.type() == DriveType::NONE)
		वापस;
	अगर (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		वापस;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::चालक[](पूर्णांक i) स्थिर
{
	DriveBitMultiple परिणाम;
	क्रम (स्वतः स्थिर &single : multiple_)
		परिणाम.merge(single[i]);
	वापस परिणाम;
}


bool DriveChunkWire::can_append(DriveBitWire स्थिर &bit) स्थिर
{
	वापस bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire स्थिर &bit)
{
	अगर (!can_append(bit))
		वापस false;
	width += 1;
	वापस true;
}

bool DriveChunkWire::try_append(DriveChunkWire स्थिर &chunk)
{
	अगर (chunk.wire != wire || chunk.offset != offset + width)
		वापस false;
	width += chunk.width;
	वापस true;
}

bool DriveChunkPort::can_append(DriveBitPort स्थिर &bit) स्थिर
{
	वापस bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort स्थिर &bit)
{
	अगर (!can_append(bit))
		वापस false;
	width += 1;
	वापस true;
}

bool DriveChunkPort::try_append(DriveChunkPort स्थिर &chunk)
{
	अगर (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		वापस false;
	width += chunk.width;
	वापस true;
}

bool DriveChunkMarker::can_append(DriveBitMarker स्थिर &bit) स्थिर
{
	वापस bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker स्थिर &bit)
{
	अगर (!can_append(bit))
		वापस false;
	width += 1;
	वापस true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker स्थिर &chunk)
{
	अगर (chunk.marker != marker || chunk.offset != offset + width)
		वापस false;
	width += chunk.width;
	वापस true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple स्थिर &bit) स्थिर
{
	अगर (bit.multiple().आकार() != multiple_.आकार())
		वापस false;

	पूर्णांक const_drivers = 0;
	क्रम (DriveChunk स्थिर &single : multiple_)
		अगर (single.is_constant())
			const_drivers += 1;

	अगर (const_drivers > 1)
		वापस false;

	क्रम (DriveBit स्थिर &single : bit.multiple())
		अगर (single.is_constant())
			const_drivers -= 1;

	अगर (const_drivers != 0)
		वापस false;

	क्रम (DriveChunk स्थिर &single : multiple_)
	{
		चयन (single.type())
		{
			हाल DriveType::CONSTANT: {
			} अवरोध;
			हाल DriveType::WIRE: {
				स्वतः स्थिर &wire = single.wire();
				DriveBit अग्र = DriveBitWire(wire.wire, wire.offset + wire.width);
				अगर (!bit.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			हाल DriveType::PORT: {
				स्वतः स्थिर &port = single.port();
				DriveBit अग्र = DriveBitPort(port.cell, port.port, port.offset + port.width);
				अगर (!bit.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			हाल DriveType::MARKER: {
				स्वतः स्थिर &marker = single.marker();
				DriveBit अग्र = DriveBitMarker(marker.marker, marker.offset + marker.width);
				अगर (!bit.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			शेष:
				वापस false;
		}
	}

	वापस true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple स्थिर &chunk) स्थिर
{
	अगर (chunk.multiple().आकार() != multiple_.आकार())
		वापस false;

	पूर्णांक const_drivers = 0;
	क्रम (DriveChunk स्थिर &single : multiple_)
		अगर (single.is_constant())
			const_drivers += 1;

	अगर (const_drivers > 1)
		वापस false;

	क्रम (DriveChunk स्थिर &single : chunk.multiple())
		अगर (single.is_constant())
			const_drivers -= 1;

	अगर (const_drivers != 0)
		वापस false;

	क्रम (DriveChunk स्थिर &single : multiple_)
	{
		चयन (single.type())
		{
			हाल DriveType::CONSTANT: {
			} अवरोध;
			हाल DriveType::WIRE: {
				स्वतः स्थिर &wire = single.wire();
				DriveChunk अग्र = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.आकार());
				अगर (!chunk.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			हाल DriveType::PORT: {
				स्वतः स्थिर &port = single.port();
				DriveChunk अग्र = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.आकार());
				अगर (!chunk.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			हाल DriveType::MARKER: {
				स्वतः स्थिर &marker = single.marker();
				DriveChunk अग्र = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.आकार());
				अगर (!chunk.multiple().गिनती(अग्र))
					वापस false;
			} अवरोध;
			शेष:
				वापस false;
		}
	}

	वापस true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple स्थिर &bit)
{
	अगर (!can_append(bit))
		वापस false;
	width_ += 1;
	State constant;

	क्रम (DriveBit स्थिर &single : bit.multiple())
		अगर (single.is_constant())
			constant = single.constant();

	क्रम (DriveChunk &single : multiple_)
	{
		चयन (single.type())
		{
			हाल DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} अवरोध;
			हाल DriveType::WIRE: {
				single.wire().width += 1;
			} अवरोध;
			हाल DriveType::PORT: {
				single.port().width += 1;
			} अवरोध;
			हाल DriveType::MARKER: {
				single.marker().width += 1;
			} अवरोध;
			शेष:
				log_abort();
		}
	}
	वापस true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple स्थिर &chunk)
{
	अगर (!can_append(chunk))
		वापस false;
	पूर्णांक width = chunk.आकार();
	width_ += width;
	Const constant;

	क्रम (DriveChunk स्थिर &single : chunk.multiple())
		अगर (single.is_constant())
			constant = single.constant();

	क्रम (DriveChunk &single : multiple_)
	{
		चयन (single.type())
		{
			हाल DriveType::CONSTANT: {
				single.constant().append(constant);
			} अवरोध;
			हाल DriveType::WIRE: {
				single.wire().width += width;
			} अवरोध;
			हाल DriveType::PORT: {
				single.port().width += width;
			} अवरोध;
			हाल DriveType::MARKER: {
				single.marker().width += width;
			} अवरोध;
			शेष:
				log_abort();
		}
	}
	वापस true;
}

bool DriveChunk::can_append(DriveBit स्थिर &bit) स्थिर
{
	अगर (आकार() == 0)
		वापस true;
	अगर (bit.type() != type_)
		वापस false;
	चयन (type_)
	{
		हाल DriveType::NONE:
			वापस true;
		हाल DriveType::CONSTANT:
			वापस true;
		हाल DriveType::WIRE:
			वापस wire_.can_append(bit.wire());
		हाल DriveType::PORT:
			वापस port_.can_append(bit.port());
		हाल DriveType::MULTIPLE:
			वापस multiple_.can_append(bit.multiple());
		शेष:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit स्थिर &bit)
{
	अगर (आकार() == 0)
		*यह = bit;
	अगर (bit.type() != type_)
		वापस false;
	चयन (type_)
	{
		हाल DriveType::NONE:
			none_ += 1;
			वापस true;
		हाल DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			वापस true;
		हाल DriveType::WIRE:
			वापस wire_.try_append(bit.wire());
		हाल DriveType::PORT:
			वापस port_.try_append(bit.port());
		हाल DriveType::MULTIPLE:
			वापस multiple_.try_append(bit.multiple());
		शेष:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk स्थिर &chunk)
{
	अगर (आकार() == 0)
		*यह = chunk;
	अगर (chunk.type_ != type_)
		वापस false;
	चयन (type_)
	{
		हाल DriveType::NONE:
			none_ += chunk.none_;
			वापस true;
		हाल DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			वापस true;
		हाल DriveType::WIRE:
			वापस wire_.try_append(chunk.wire());
		हाल DriveType::PORT:
			वापस port_.try_append(chunk.port());
		हाल DriveType::MARKER:
			वापस marker_.try_append(chunk.marker());
		हाल DriveType::MULTIPLE:
			वापस multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

व्योम DriveSpec::append(DriveBit स्थिर &bit)
{
	hash_ = 0;
	अगर (!packed()) {
		bits_.धकेलो_back(bit);
		width_ += 1;
		वापस;
	}

	अगर (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

व्योम DriveSpec::append(DriveChunk स्थिर &chunk)
{
	hash_ = 0;
	pack();
	अगर (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.आकार();
}

व्योम DriveSpec::pack() स्थिर {
	अगर (bits_.empty())
		वापस;
	std::vector<DriveBit> bits(std::move(bits_));
	क्रम (स्वतः &bit : bits)
		अगर (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

व्योम DriveSpec::unpack() स्थिर {
	अगर (chunks_.empty())
		वापस;
	क्रम (स्वतः &chunk : chunks_)
	{
		क्रम (पूर्णांक i = 0, width = chunk.आकार(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.सफा();
}

व्योम DriveSpec::compute_width()
{
	width_ = 0;
	क्रम (स्वतः स्थिर &chunk : chunks_)
		width_ += chunk.आकार();
}

व्योम DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	अगर (first_edges.emplace(src, dst).first->second == dst)
		वापस;
	अगर (second_edges.emplace(src, dst).first->second == dst)
		वापस;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::निकालो_edge(DriveBitId src)
{
	// TODO unused I think?
	स्वतः found_more = more_edges.find(src);
	अगर (found_more != more_edges.end()) {
		स्वतः परिणाम = found_more->second.निकालो();
		अगर (found_more->second.empty())
			more_edges.erase(found_more);
		वापस परिणाम;
	}

	स्वतः found_second = second_edges.find(src);
	अगर (found_second != second_edges.end()) {
		स्वतः परिणाम = found_second->second;
		second_edges.erase(found_second);
		वापस परिणाम;
	}

	स्वतः found_first = first_edges.find(src);
	अगर (found_first != first_edges.end()) {
		स्वतः परिणाम = found_first->second;
		first_edges.erase(found_first);
		वापस परिणाम;
	}

	वापस DriveBitId();
}

व्योम DriverMap::DriveBitGraph::सफा(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	वापस first_edges.गिनती(src);
}

पूर्णांक DriverMap::DriveBitGraph::गिनती(DriveBitId src)
{
	अगर (!first_edges.गिनती(src))
		वापस 0;
	अगर (!second_edges.गिनती(src))
		वापस 1;
	स्वतः found = more_edges.find(src);
	अगर (found == more_edges.end())
		वापस 2;
	वापस Get_आकार(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णांक सूचकांक)
{
	अगर (सूचकांक == 0)
		वापस first_edges.at(src);
	अन्यथा अगर (सूचकांक == 1)
		वापस second_edges.at(src);
	अन्यथा
		वापस *more_edges.at(src).element(सूचकांक - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit स्थिर &bit)
{
	चयन (bit.type())
	{
		हाल DriveType::NONE:
			वापस BitMode::NONE;
		हाल DriveType::CONSTANT:
			// TODO how to handle Sx here?
			वापस bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		हाल DriveType::WIRE: {
			स्वतः स्थिर &wire = bit.wire();
			bool driver = wire.wire->port_निवेश;
			bool driven = wire.wire->port_output;

			अगर (driver && !driven)
				वापस BitMode::DRIVER;
			अन्यथा अगर (driven && !driver)
				वापस BitMode::DRIVEN;
			अन्यथा अगर (driver && driven)
				वापस BitMode::TRISTATE;
			अन्यथा
				वापस keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		हाल DriveType::PORT: {
			स्वतः स्थिर &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_निवेश(port.cell->type, port.port);
			अगर (driver && !driven)
				वापस BitMode::DRIVER;
			अन्यथा अगर (driven && !driver)
				वापस BitMode::DRIVEN_UNIQUE;
			अन्यथा
				वापस BitMode::TRISTATE;
		}
		हाल DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		शेष:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit स्थिर &bit)
{
	चयन (bit.type())
	{
		हाल DriveType::NONE:
			वापस -1;
		हाल DriveType::CONSTANT:
			वापस (पूर्णांक)bit.constant();
		हाल DriveType::WIRE: {
			स्वतः स्थिर &wire_bit = bit.wire();
			पूर्णांक offset = अग्र_offset;
			स्वतः insertion = wire_offsets.emplace(wire_bit.wire, offset);
			अगर (insertion.second) {
				अगर (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				अग्र_offset += wire_bit.wire->width;
			}
			वापस insertion.first->second.id + wire_bit.offset;
		}
		हाल DriveType::PORT: {
			स्वतः स्थिर &port_bit = bit.port();
			स्वतः कुंजी = std::make_pair(port_bit.cell, port_bit.port);
			पूर्णांक offset = अग्र_offset;
			स्वतः insertion = port_offsets.emplace(कुंजी, offset);
			अगर (insertion.second) {
				पूर्णांक width = port_bit.cell->connections().at(port_bit.port).आकार();
				अगर (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} अन्यथा
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				अग्र_offset += width;
			}
			वापस insertion.first->second.id + port_bit.offset;
		}
		शेष:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	स्वतः found_isolated = isolated_drive_bits.find(id);
	अगर (found_isolated != isolated_drive_bits.end())
		वापस found_isolated->second;

	स्वतः found = drive_bits.upper_bound(id);
	अगर (found == drive_bits.begin()) {
		वापस id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit परिणाम = found->second;
	अगर (परिणाम.is_wire()) {
		परिणाम.wire().offset += id.id - found->first.id;
	} अन्यथा {
		log_assert(परिणाम.is_port());
		परिणाम.port().offset += id.id - found->first.id;
	}
	वापस परिणाम;
}

व्योम DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	अगर (driven_id == driver_id)
		वापस;

	same_driver.merge(driven_id, driver_id);

	क्रम (पूर्णांक i = 0, end = connected_drivers.गिनती(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.सफा(driven_id);

	क्रम (पूर्णांक i = 0, end = connected_undirected.गिनती(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.सफा(driven_id);
}

व्योम DriverMap::connect_directed_बफर(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

व्योम DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

व्योम DriverMap::add(Module *module)
{
	क्रम (स्वतः स्थिर &conn : module->connections())
		add(conn.first, conn.second);

	क्रम (स्वतः cell : module->cells())
		क्रम (स्वतः स्थिर &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
व्योम DriverMap::add(DriveBit स्थिर &a, DriveBit स्थिर &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	अगर (a_id == b_id)
		वापस;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	अगर (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	अन्यथा अगर (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	अन्यथा अगर (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_बफर(a_id, b_id);
	अन्यथा अगर (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_बफर(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	अन्यथा अगर (a_mode == BitMode::DRIVER)
		connect_directed_बफर(b_id, a_id);
	अन्यथा अगर (b_mode == BitMode::DRIVER)
		connect_directed_बफर(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	अन्यथा
		connect_undirected(a_id, b_id);

	वापस;
}

// Specialized version that avoids unpacking
व्योम DriverMap::add(SigSpec स्थिर &a, SigSpec स्थिर &b)
{
	log_assert(a.आकार() == b.आकार());
	स्वतः स्थिर &a_chunks = a.chunks();
	स्वतः स्थिर &b_chunks = b.chunks();

	स्वतः a_chunk = a_chunks.begin();
	स्वतः a_end = a_chunks.end();
	पूर्णांक a_offset = 0;

	स्वतः b_chunk = b_chunks.begin();
	पूर्णांक b_offset = 0;

	SigChunk tmp_a, tmp_b;
	जबतक (a_chunk != a_end) {
		पूर्णांक a_width = a_chunk->width - a_offset;
		अगर (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			जारी;
		}
		पूर्णांक b_width = b_chunk->width - b_offset;
		अगर (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			जारी;
		}
		पूर्णांक width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk स्थिर &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk स्थिर &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

व्योम DriverMap::add_port(Cell *cell, IdString port, SigSpec स्थिर &b)
{
	पूर्णांक offset = 0;
	क्रम (स्वतः स्थिर &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.आकार();
	}
}

व्योम DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, पूर्णांक> &distance = orient_undirected_distance;
	seen.सफा();
	drivers.सफा();

	seen.emplace(id);

	क्रम (पूर्णांक pos = 0; pos < Get_आकार(seen); ++pos) {
		DriveBitId current = *seen.element(seen.आकार() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		अगर (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		अगर (connected_drivers.contains(current))
			drivers.emplace(current);

		पूर्णांक undirected_driver_गिनती = connected_undirected.गिनती(current);

		क्रम (पूर्णांक i = 0; i != undirected_driver_गिनती; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	अगर (drivers.empty())
		क्रम (स्वतः seen_id : seen)
			drivers.emplace(seen_id);

	क्रम (स्वतः driver : drivers)
	{
		distance.सफा();
		distance.emplace(driver, 0);

		क्रम (पूर्णांक pos = 0; pos < Get_आकार(distance); ++pos) {
			स्वतः current_it = distance.element(distance.आकार() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णांक undirected_driver_गिनती = connected_undirected.गिनती(current);

			क्रम (पूर्णांक i = 0; i != undirected_driver_गिनती; ++i)
			{
				DriveBitId अग्र = same_driver.find(connected_undirected.at(current, i));
				स्वतः emplaced = distance.emplace(अग्र, current_it->second + 1);
				अगर (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(अग्र, current);
			}
		}
	}

	क्रम (स्वतः seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::चालक()(DriveBit स्थिर &bit)
{
	अगर (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		वापस bit;
	अगर (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit परिणाम;
		क्रम (स्वतः स्थिर &inner : bit.multiple().multiple())
			परिणाम.merge((*यह)(inner));
		वापस परिणाम;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	अगर (mode == BitMode::KEEP && bit_repr_id != bit_id)
		वापस bit_repr;

	पूर्णांक implicit_driver_गिनती = connected_drivers.गिनती(bit_repr_id);
	अगर (connected_undirected.contains(bit_repr_id) && !oriented_present.गिनती(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	अगर (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	क्रम (पूर्णांक i = 0; i != implicit_driver_गिनती; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णांक oriented_driver_गिनती = connected_oriented.गिनती(bit_repr_id);
	क्रम (पूर्णांक i = 0; i != oriented_driver_गिनती; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	वापस driver;
}

DriveSpec DriverMap::चालक()(DriveSpec spec)
{
	DriveSpec परिणाम;

	क्रम (पूर्णांक i = 0, width = spec.आकार(); i != width; ++i)
		परिणाम.append((*यह)(spec[i]));

	वापस परिणाम;
}

std::string log_signal(DriveChunkWire स्थिर &chunk)
{
	std::string id = chunk.wire->name.unescape();
	अगर (chunk.is_whole())
		वापस id;
	अगर (chunk.width == 1)
		वापस stringf("%s [%d]", id, chunk.offset);
	वापस stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort स्थिर &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	अगर (chunk.is_whole())
		वापस stringf("%s <%s>", cell_id, port_id);
	अगर (chunk.width == 1)
		वापस stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	वापस stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker स्थिर &chunk)
{
	अगर (chunk.width == 1)
		वापस stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	वापस stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk स्थिर &chunk)
{
	चयन (chunk.type())
	{
		हाल DriveType::NONE:
			वापस stringf("<none x%d>", chunk.आकार());
		हाल DriveType::CONSTANT:
			वापस log_const(chunk.constant());
		हाल DriveType::WIRE:
			वापस log_signal(chunk.wire());
		हाल DriveType::PORT:
			वापस log_signal(chunk.port());
		हाल DriveType::MARKER:
			वापस log_signal(chunk.marker());
		हाल DriveType::MULTIPLE: {
			std::string str = "<multiple";
			स्थिर अक्षर *sep = " ";
			क्रम (स्वतः स्थिर &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			वापस str;
		}
		शेष:
			log_abort();
	}
}

std::string log_signal(DriveSpec स्थिर &spec)
{
	स्वतः &chunks = spec.chunks();
	अगर (chunks.empty())
		वापस "{}";
	अगर (chunks.आकार() == 1)
		वापस log_signal(chunks[0]);

	std::string str;
	स्थिर अक्षर *sep = "{ ";

	क्रम (स्वतः i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	वापस str;
}

YOSYS_NAMESPACE_END
