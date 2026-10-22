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

#నిర్వచితం_కాకపోతే DRIVERTOOLS_H
#నిర్వచించు DRIVERTOOLS_H

#అన్తర్భావ <string>
#అన్తర్భావ <type_traits>

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

నిర్మాణం Drive_బిట్టు;

నిర్మాణం DriveChunkWire;
నిర్మాణం Drive_Chunk_పోర్టు;
నిర్మాణం DriveChunkMarker;
నిర్మాణం DriveChunk;

నిర్మాణం DriveSpec;

std::string log_signal(DriveChunkWire స్థిరరాశి &chunk);
std::string log_signal(Drive_Chunk_పోర్టు స్థిరరాశి &chunk);
std::string log_signal(DriveChunkMarker స్థిరరాశి &chunk);
std::string log_signal(DriveChunk స్థిరరాశి &chunk);
std::string log_signal(DriveSpec స్థిరరాశి &chunk);

గణన class DriveType : గుర్తులేని అక్షరం
{
	NONE,
	CONSTANT,
	WIRE,
	పోర్టు_2,
	గుణిజం,
	MARKER,
};

నిర్మాణం Drive_బిట్టు_Wire
{
	Wire *wire;
	పూర్ణాంకం స్థానభ్రంశం;

	Drive_బిట్టు_Wire(Wire *wire, పూర్ణాంకం స్థానభ్రంశం) : wire(wire), స్థానభ్రంశం(స్థానభ్రంశం) {}

	bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు_Wire &other) స్థిరరాశి
	{
		తిరిగి wire == other.wire && స్థానభ్రంశం == other.స్థానభ్రంశం;
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_బిట్టు_Wire &other) స్థిరరాశి
	{
		ఒకవేళ (wire != other.wire)
			తిరిగి wire->name < other.wire->name;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;


	క్రియాకారకం Sig_బిట్టు() స్థిరరాశి
	{
		తిరిగి Sig_బిట్టు(wire, స్థానభ్రంశం);
	}
};

నిర్మాణం Drive_బిట్టు_పోర్టు
{
	Cell *cell;
	IdString పోర్టు;
	పూర్ణాంకం స్థానభ్రంశం;

	Drive_బిట్టు_పోర్టు(Cell *cell, IdString పోర్టు, పూర్ణాంకం స్థానభ్రంశం) : cell(cell), పోర్టు(పోర్టు), స్థానభ్రంశం(స్థానభ్రంశం) {}

	bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు_పోర్టు &other) స్థిరరాశి
	{
		తిరిగి cell == other.cell && పోర్టు == other.పోర్టు && స్థానభ్రంశం == other.స్థానభ్రంశం;
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_బిట్టు_పోర్టు &other) స్థిరరాశి
	{
		ఒకవేళ (cell != other.cell)
			తిరిగి cell->name < other.cell->name;
		ఒకవేళ (పోర్టు != other.పోర్టు)
			తిరిగి పోర్టు < other.పోర్టు;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

};


నిర్మాణం Drive_బిట్టు_Marker
{
	పూర్ణాంకం marker;
	పూర్ణాంకం స్థానభ్రంశం;

	Drive_బిట్టు_Marker(పూర్ణాంకం marker, పూర్ణాంకం స్థానభ్రంశం) : marker(marker), స్థానభ్రంశం(స్థానభ్రంశం) {}

	bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు_Marker &other) స్థిరరాశి
	{
		తిరిగి marker == other.marker && స్థానభ్రంశం == other.స్థానభ్రంశం;
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_బిట్టు_Marker &other) స్థిరరాశి
	{
		ఒకవేళ (marker != other.marker)
			తిరిగి marker < other.marker;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

};

నిర్మాణం Drive_బిట్టు_గుణిజం
{
అంతరంగ:
	pool<Drive_బిట్టు> గుణిజం_3;

బహిరంగ:
	Drive_బిట్టు_గుణిజం();
	Drive_బిట్టు_గుణిజం(Drive_బిట్టు స్థిరరాశి &single);

	pool<Drive_బిట్టు> స్థిరరాశి &గుణిజం_2() స్థిరరాశి { తిరిగి గుణిజం_3; }

	శూన్య విలీనం(Drive_బిట్టు_గుణిజం స్థిరరాశి &other)
	{
		కొరకు (Drive_బిట్టు స్థిరరాశి &single : other.గుణిజం_3)
			విలీనం(single);
	}

	శూన్య విలీనం(Drive_బిట్టు_గుణిజం &&other)
	{
		కొరకు (Drive_బిట్టు &single : other.గుణిజం_3)
			విలీనం(std::move(single));
	}

	శూన్య విలీనం(Drive_బిట్టు స్థిరరాశి &single);
	శూన్య విలీనం(Drive_బిట్టు &&single);

	bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు_గుణిజం &other) స్థిరరాశి
	{
		తిరిగి గుణిజం_3 == other.గుణిజం_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

నిర్మాణం Drive_బిట్టు
{
అంతరంగ:
	DriveType type_ = DriveType::NONE;
	సంఘం
	{
		State constant_;
		Drive_బిట్టు_Wire wire_;
		Drive_బిట్టు_పోర్టు పోర్టు_3;
		Drive_బిట్టు_Marker marker_;
		Drive_బిట్టు_గుణిజం గుణిజం_3;
	};
బహిరంగ:
	Drive_బిట్టు()  {}

	Drive_బిట్టు(Sig_బిట్టు స్థిరరాశి &బిట్టు);

	Drive_బిట్టు(Drive_బిట్టు స్థిరరాశి &other) { *this = other; }
	Drive_బిట్టు(Drive_బిట్టు &&other) { *this = other; }


	Drive_బిట్టు(State constant) { *this = constant; }
	Drive_బిట్టు(Drive_బిట్టు_Wire స్థిరరాశి &wire) { *this = wire; }
	Drive_బిట్టు(Drive_బిట్టు_Wire &&wire) { *this = wire; }
	Drive_బిట్టు(Drive_బిట్టు_పోర్టు స్థిరరాశి &పోర్టు) { *this = పోర్టు; }
	Drive_బిట్టు(Drive_బిట్టు_పోర్టు &&పోర్టు) { *this = పోర్టు; }
	Drive_బిట్టు(Drive_బిట్టు_Marker స్థిరరాశి &marker) { *this = marker; }
	Drive_బిట్టు(Drive_బిట్టు_Marker &&marker) { *this = marker; }
	Drive_బిట్టు(Drive_బిట్టు_గుణిజం స్థిరరాశి &గుణిజం_2) { *this = గుణిజం_2; }
	Drive_బిట్టు(Drive_బిట్టు_గుణిజం &&గుణిజం_2) { *this = గుణిజం_2; }

	~Drive_బిట్టు() { సమితి_none(); }

	శూన్య సమితి_none()
	{
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				ఆపు;
			స్థితి DriveType::CONSTANT:
				ఆపు;
			స్థితి DriveType::WIRE:
				wire_.~Drive_బిట్టు_Wire();
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				పోర్టు_3.~Drive_బిట్టు_పోర్టు();
				ఆపు;
			స్థితి DriveType::MARKER:
				marker_.~Drive_బిట్టు_Marker();
				ఆపు;
			స్థితి DriveType::గుణిజం:
				గుణిజం_3.~Drive_బిట్టు_గుణిజం();
				ఆపు;
		}
		type_ = DriveType::NONE;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు స్థిరరాశి &other)
	{
		ఎంపిక (other.type_)
		{
			స్థితి DriveType::NONE:
				సమితి_none();
				ఆపు;
			స్థితి DriveType::CONSTANT:
				*this = other.constant_;
				ఆపు;
			స్థితి DriveType::WIRE:
				*this = other.wire_;
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				*this = other.పోర్టు_3;
				ఆపు;
			స్థితి DriveType::MARKER:
				*this = other.marker_;
				ఆపు;
			స్థితి DriveType::గుణిజం:
				*this = other.గుణిజం_3;
				ఆపు;
		}
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు &&other)
	{
		ఎంపిక (other.type_)
		{
			స్థితి DriveType::NONE:
				సమితి_none();
				ఆపు;
			స్థితి DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ఆపు;
			స్థితి DriveType::WIRE:
				*this = std::move(other.wire_);
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				*this = std::move(other.పోర్టు_3);
				ఆపు;
			స్థితి DriveType::MARKER:
				*this = std::move(other.marker_);
				ఆపు;
			స్థితి DriveType::గుణిజం:
				*this = std::move(other.గుణిజం_3);
				ఆపు;
		}
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(State constant)
	{
		సమితి_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_Wire స్థిరరాశి &wire)
	{
		సమితి_none();
		new (&wire_) Drive_బిట్టు_Wire(wire);
		type_ = DriveType::WIRE;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_Wire &&wire)
	{
		సమితి_none();
		new (&wire_) Drive_బిట్టు_Wire(wire);
		type_ = DriveType::WIRE;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_పోర్టు స్థిరరాశి &పోర్టు)
	{
		సమితి_none();
		new (&పోర్టు_3) Drive_బిట్టు_పోర్టు(పోర్టు);
		type_ = DriveType::పోర్టు_2;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_పోర్టు &&పోర్టు)
	{
		సమితి_none();
		new (&పోర్టు_3) Drive_బిట్టు_పోర్టు(పోర్టు);
		type_ = DriveType::పోర్టు_2;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_Marker స్థిరరాశి &marker)
	{
		సమితి_none();
		new (&marker_) Drive_బిట్టు_Marker(marker);
		type_ = DriveType::MARKER;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_Marker &&marker)
	{
		సమితి_none();
		new (&marker_) Drive_బిట్టు_Marker(marker);
		type_ = DriveType::MARKER;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_గుణిజం స్థిరరాశి &గుణిజం_2)
	{
		సమితి_none();
		ఒకవేళ (గుణిజం_2.గుణిజం_2().ఖాళీలు())
			తిరిగి *this;
		new (&గుణిజం_3) Drive_బిట్టు_గుణిజం(గుణిజం_2);
		type_ = DriveType::గుణిజం;
		తిరిగి *this;
	}

	Drive_బిట్టు &క్రియాకారకం=(Drive_బిట్టు_గుణిజం &&గుణిజం_2)
	{
		సమితి_none();
		ఒకవేళ (గుణిజం_2.గుణిజం_2().ఖాళీలు())
			తిరిగి *this;
		new (&గుణిజం_3) Drive_బిట్టు_గుణిజం(గుణిజం_2);
		type_ = DriveType::గుణిజం;
		తిరిగి *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

	bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు &other) స్థిరరాశి
	{
		ఒకవేళ (type_ != other.type_)
			తిరిగి false;

		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి true;
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_ == other.constant_;
			స్థితి DriveType::WIRE:
				తిరిగి wire_ == other.wire_;
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3 == other.పోర్టు_3;
			స్థితి DriveType::MARKER:
				తిరిగి marker_ == other.marker_;
			స్థితి DriveType::గుణిజం:
				తిరిగి గుణిజం_3 == other.గుణిజం_3;
		}
		log_abort();
	}

	bool క్రియాకారకం!=(స్థిరరాశి Drive_బిట్టు &other) స్థిరరాశి
	{
		తిరిగి !(*this == other);
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_బిట్టు &other) స్థిరరాశి
	{
		ఒకవేళ (type_ != other.type_)
			తిరిగి type_ < other.type_;
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి false;
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_ < other.constant_;
			స్థితి DriveType::WIRE:
				తిరిగి wire_ < other.wire_;
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3 < other.పోర్టు_3;
			స్థితి DriveType::MARKER:
				తిరిగి marker_ < other.marker_;
			స్థితి DriveType::గుణిజం:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() స్థిరరాశి { తిరిగి type_; }

	bool is_none() స్థిరరాశి { తిరిగి type_ == DriveType::NONE; }
	bool is_constant() స్థిరరాశి { తిరిగి type_ == DriveType::CONSTANT; }
	bool is_wire() స్థిరరాశి { తిరిగి type_ == DriveType::WIRE; }
	bool is_పోర్టు() స్థిరరాశి { తిరిగి type_ == DriveType::పోర్టు_2; }
	bool is_marker() స్థిరరాశి { తిరిగి type_ == DriveType::MARKER; }
	bool is_గుణిజం() స్థిరరాశి { తిరిగి type_ == DriveType::గుణిజం; }

	State &constant() { log_assert(is_constant()); తిరిగి constant_; }
	State స్థిరరాశి &constant() స్థిరరాశి { log_assert(is_constant()); తిరిగి constant_; }
	Drive_బిట్టు_Wire &wire() { log_assert(is_wire()); తిరిగి wire_; }
	Drive_బిట్టు_Wire స్థిరరాశి &wire() స్థిరరాశి { log_assert(is_wire()); తిరిగి wire_; }
	Drive_బిట్టు_పోర్టు &పోర్టు() { log_assert(is_పోర్టు()); తిరిగి పోర్టు_3; }
	Drive_బిట్టు_పోర్టు స్థిరరాశి &పోర్టు() స్థిరరాశి { log_assert(is_పోర్టు()); తిరిగి పోర్టు_3; }
	Drive_బిట్టు_Marker &marker() { log_assert(is_marker()); తిరిగి marker_; }
	Drive_బిట్టు_Marker స్థిరరాశి &marker() స్థిరరాశి { log_assert(is_marker()); తిరిగి marker_; }
	Drive_బిట్టు_గుణిజం &గుణిజం_2() { log_assert(is_గుణిజం()); తిరిగి గుణిజం_3; }
	Drive_బిట్టు_గుణిజం స్థిరరాశి &గుణిజం_2() స్థిరరాశి { log_assert(is_గుణిజం()); తిరిగి గుణిజం_3; }

	శూన్య విలీనం(Drive_బిట్టు స్థిరరాశి &other);

};

అంతఃవరుస Drive_బిట్టు_గుణిజం::Drive_బిట్టు_గుణిజం() {}
అంతఃవరుస Drive_బిట్టు_గుణిజం::Drive_బిట్టు_గుణిజం(Drive_బిట్టు స్థిరరాశి &single)
{
	గుణిజం_3.emplace(single);
}

నిర్మాణం DriveChunkWire
{
	Wire *wire;
	పూర్ణాంకం స్థానభ్రంశం;
	పూర్ణాంకం వెడల్పు;

	DriveChunkWire(Wire *wire, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు) : wire(wire), స్థానభ్రంశం(స్థానభ్రంశం), వెడల్పు(వెడల్పు) {}
	DriveChunkWire(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు) : wire(బిట్టు.wire), స్థానభ్రంశం(బిట్టు.స్థానభ్రంశం), వెడల్పు(1) {}

	పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు; }

	Drive_బిట్టు_Wire క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి
	{
		log_assert(i >= 0 && i < వెడల్పు);
		తిరిగి Drive_బిట్టు_Wire(wire, స్థానభ్రంశం + i);
	}

	bool can_append(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు) స్థిరరాశి;
	bool try_append(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు);
	bool try_append(DriveChunkWire స్థిరరాశి &chunk);

	// Whether this chunk is a whole wire
	bool is_సంపూర్ణ() స్థిరరాశి { తిరిగి స్థానభ్రంశం == 0 && వెడల్పు == wire->వెడల్పు; }

	bool క్రియాకారకం==(స్థిరరాశి DriveChunkWire &other) స్థిరరాశి
	{
		తిరిగి wire == other.wire && స్థానభ్రంశం == other.స్థానభ్రంశం && వెడల్పు == other.వెడల్పు;
	}

	bool క్రియాకారకం<(స్థిరరాశి DriveChunkWire &other) స్థిరరాశి
	{
		ఒకవేళ (wire != other.wire)
			తిరిగి wire->name < other.wire->name;
		ఒకవేళ (వెడల్పు != other.వెడల్పు)
			తిరిగి వెడల్పు < other.వెడల్పు;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

	explicit క్రియాకారకం SigChunk() స్థిరరాశి
	{
		తిరిగి SigChunk(wire, స్థానభ్రంశం, వెడల్పు);
	}
};

నిర్మాణం Drive_Chunk_పోర్టు
{
	Cell *cell;
	IdString పోర్టు;
	పూర్ణాంకం స్థానభ్రంశం;
	పూర్ణాంకం వెడల్పు;

	Drive_Chunk_పోర్టు(Cell *cell, IdString పోర్టు, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు) :
		cell(cell), పోర్టు(పోర్టు), స్థానభ్రంశం(స్థానభ్రంశం), వెడల్పు(వెడల్పు) { }
	Drive_Chunk_పోర్టు(Cell *cell, IdString పోర్టు) :
		cell(cell), పోర్టు(పోర్టు), స్థానభ్రంశం(0), వెడల్పు(Get_కొలత(cell->connections().at(పోర్టు))) { }
	Drive_Chunk_పోర్టు(Cell *cell, std::pair<IdString, SigSpec> స్థిరరాశి &conn) :
		cell(cell), పోర్టు(conn.మొదటి), స్థానభ్రంశం(0), వెడల్పు(Get_కొలత(conn.రెండవ)) { }
	Drive_Chunk_పోర్టు(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు) :
		cell(బిట్టు.cell), పోర్టు(బిట్టు.పోర్టు), స్థానభ్రంశం(బిట్టు.స్థానభ్రంశం), వెడల్పు(1) { }

	పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు; }

	Drive_బిట్టు_పోర్టు క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి
	{
		log_assert(i >= 0 && i < వెడల్పు);
		తిరిగి Drive_బిట్టు_పోర్టు(cell, పోర్టు, స్థానభ్రంశం + i);
	}

	bool can_append(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు) స్థిరరాశి;
	bool try_append(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు);
	bool try_append(Drive_Chunk_పోర్టు స్థిరరాశి &chunk);

	// Whether this chunk is a whole port
	bool is_సంపూర్ణ() స్థిరరాశి
	{
		తిరిగి స్థానభ్రంశం == 0 && వెడల్పు == cell->connections().at(పోర్టు).కొలత();
	}

	bool క్రియాకారకం==(స్థిరరాశి Drive_Chunk_పోర్టు &other) స్థిరరాశి
	{
		తిరిగి cell == other.cell && పోర్టు == other.పోర్టు && స్థానభ్రంశం == other.స్థానభ్రంశం && వెడల్పు == other.వెడల్పు;
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_Chunk_పోర్టు &other) స్థిరరాశి
	{
		ఒకవేళ (cell != other.cell)
			తిరిగి cell->name < other.cell->name;
		ఒకవేళ (పోర్టు != other.పోర్టు)
			తిరిగి పోర్టు < other.పోర్టు;
		ఒకవేళ (వెడల్పు != other.వెడల్పు)
			తిరిగి వెడల్పు < other.వెడల్పు;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};


నిర్మాణం DriveChunkMarker
{
	పూర్ణాంకం marker;
	పూర్ణాంకం స్థానభ్రంశం;
	పూర్ణాంకం వెడల్పు;

	DriveChunkMarker(పూర్ణాంకం marker, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు) :
		marker(marker), స్థానభ్రంశం(స్థానభ్రంశం), వెడల్పు(వెడల్పు) {}
	DriveChunkMarker(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు) :
		marker(బిట్టు.marker), స్థానభ్రంశం(బిట్టు.స్థానభ్రంశం), వెడల్పు(1) {}

	పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు; }

	Drive_బిట్టు_Marker క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి
	{
		log_assert(i >= 0 && i < వెడల్పు);
		తిరిగి Drive_బిట్టు_Marker(marker, స్థానభ్రంశం + i);
	}

	bool can_append(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు) స్థిరరాశి;
	bool try_append(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు);
	bool try_append(DriveChunkMarker స్థిరరాశి &chunk);

	bool క్రియాకారకం==(స్థిరరాశి DriveChunkMarker &other) స్థిరరాశి
	{
		తిరిగి marker == other.marker && స్థానభ్రంశం == other.స్థానభ్రంశం && వెడల్పు == other.వెడల్పు;
	}

	bool క్రియాకారకం<(స్థిరరాశి DriveChunkMarker &other) స్థిరరాశి
	{
		ఒకవేళ (marker != other.marker)
			తిరిగి marker < other.marker;
		ఒకవేళ (వెడల్పు != other.వెడల్పు)
			తిరిగి వెడల్పు < other.వెడల్పు;
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

నిర్మాణం Drive_Chunk_గుణిజం
{
అంతరంగ:
	mutable pool<DriveChunk> గుణిజం_3;
	పూర్ణాంకం వెడల్పు_3;

బహిరంగ:
	pool<DriveChunk> స్థిరరాశి &గుణిజం_2() స్థిరరాశి { తిరిగి గుణిజం_3; }

	Drive_Chunk_గుణిజం(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు);

	పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు_3; }

	Drive_బిట్టు_గుణిజం క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి;

	bool can_append(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు) స్థిరరాశి;

	bool try_append(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు);


	bool can_append(Drive_Chunk_గుణిజం స్థిరరాశి &బిట్టు) స్థిరరాశి;

	bool try_append(Drive_Chunk_గుణిజం స్థిరరాశి &బిట్టు);

	bool క్రియాకారకం==(స్థిరరాశి Drive_Chunk_గుణిజం &other) స్థిరరాశి
	{
		తిరిగి వెడల్పు_3 == other.వెడల్పు_3 && గుణిజం_3 == other.గుణిజం_3;
	}

	bool క్రియాకారకం<(స్థిరరాశి Drive_Chunk_గుణిజం &other) స్థిరరాశి
	{
		ఒకవేళ (గుణిజం_3.కొలత() < other.గుణిజం_3.కొలత())

		గుణిజం_3.క్రమీకరించు();
		తిరిగి false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

నిర్మాణం DriveChunk
{
అంతరంగ:
	DriveType type_ = DriveType::NONE;
	సంఘం
	{
		పూర్ణాంకం none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_పోర్టు పోర్టు_3;
		DriveChunkMarker marker_;
		Drive_Chunk_గుణిజం గుణిజం_3;
	};

బహిరంగ:
	DriveChunk() { సమితి_none(); }

	DriveChunk(DriveChunk స్థిరరాశి &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_బిట్టు స్థిరరాశి &other) { *this = other; }

	DriveChunk(Const స్థిరరాశి &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire స్థిరరాశి &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_పోర్టు స్థిరరాశి &పోర్టు) { *this = పోర్టు; }
	DriveChunk(Drive_Chunk_పోర్టు &&పోర్టు) { *this = పోర్టు; }
	DriveChunk(DriveChunkMarker స్థిరరాశి &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_గుణిజం స్థిరరాశి &గుణిజం_2) { *this = గుణిజం_2; }
	DriveChunk(Drive_Chunk_గుణిజం &&గుణిజం_2) { *this = గుణిజం_2; }

	~DriveChunk() { సమితి_none(); }

	Drive_బిట్టు క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి
	{
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి Drive_బిట్టు();
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_[i];
			స్థితి DriveType::WIRE:
				తిరిగి wire_[i];
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3[i];
			స్థితి DriveType::MARKER:
				తిరిగి marker_[i];
			స్థితి DriveType::గుణిజం:
				తిరిగి గుణిజం_3[i];
		}
		log_abort();
	}

	శూన్య సమితి_none(పూర్ణాంకం వెడల్పు = 0)
	{
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				none_ = వెడల్పు;
				ఆపు;
			స్థితి DriveType::CONSTANT:
				constant_.~Const();
				ఆపు;
			స్థితి DriveType::WIRE:
				wire_.~DriveChunkWire();
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				పోర్టు_3.~Drive_Chunk_పోర్టు();
				ఆపు;
			స్థితి DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ఆపు;
			స్థితి DriveType::గుణిజం:
				గుణిజం_3.~Drive_Chunk_గుణిజం();
				ఆపు;
		}
		type_ = DriveType::NONE;
		none_ = వెడల్పు;
	}

	DriveChunk &క్రియాకారకం=(DriveChunk స్థిరరాశి &other)
	{
		ఎంపిక (other.type_)
		{
			స్థితి DriveType::NONE:
				సమితి_none(other.none_);
				ఆపు;
			స్థితి DriveType::CONSTANT:
				*this = other.constant_;
				ఆపు;
			స్థితి DriveType::WIRE:
				*this = other.wire_;
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				*this = other.పోర్టు_3;
				ఆపు;
			స్థితి DriveType::MARKER:
				*this = other.marker_;
				ఆపు;
			స్థితి DriveType::గుణిజం:
				*this = other.గుణిజం_3;
				ఆపు;
		}
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(DriveChunk &&other)
	{
		ఎంపిక (other.type_)
		{
			స్థితి DriveType::NONE:
				సమితి_none(other.none_);
				ఆపు;
			స్థితి DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ఆపు;
			స్థితి DriveType::WIRE:
				*this = std::move(other.wire_);
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				*this = std::move(other.పోర్టు_3);
				ఆపు;
			స్థితి DriveType::MARKER:
				*this = std::move(other.marker_);
				ఆపు;
			స్థితి DriveType::గుణిజం:
				*this = std::move(other.గుణిజం_3);
				ఆపు;
		}
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Const స్థిరరాశి &constant)
	{
		సమితి_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Const &&constant)
	{
		సమితి_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(DriveChunkWire స్థిరరాశి &wire)
	{
		సమితి_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(DriveChunkWire &&wire)
	{
		సమితి_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Drive_Chunk_పోర్టు స్థిరరాశి &పోర్టు)
	{
		సమితి_none();
		new (&పోర్టు_3) Drive_Chunk_పోర్టు(పోర్టు);
		type_ = DriveType::పోర్టు_2;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Drive_Chunk_పోర్టు &&పోర్టు)
	{
		సమితి_none();
		new (&పోర్టు_3) Drive_Chunk_పోర్టు(పోర్టు);
		type_ = DriveType::పోర్టు_2;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(DriveChunkMarker స్థిరరాశి &marker)
	{
		సమితి_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(DriveChunkMarker &&marker)
	{
		సమితి_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Drive_Chunk_గుణిజం స్థిరరాశి &గుణిజం_2)
	{
		సమితి_none(గుణిజం_2.కొలత());
		ఒకవేళ (గుణిజం_2.గుణిజం_2().ఖాళీలు())
			తిరిగి *this;
		new (&గుణిజం_3) Drive_Chunk_గుణిజం(గుణిజం_2);
		type_ = DriveType::గుణిజం;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Drive_Chunk_గుణిజం &&గుణిజం_2)
	{
		సమితి_none(గుణిజం_2.కొలత());
		ఒకవేళ (గుణిజం_2.గుణిజం_2().ఖాళీలు())
			తిరిగి *this;
		new (&గుణిజం_3) Drive_Chunk_గుణిజం(గుణిజం_2);
		type_ = DriveType::గుణిజం;
		తిరిగి *this;
	}

	DriveChunk &క్రియాకారకం=(Drive_బిట్టు స్థిరరాశి &other)
	{
		ఎంపిక (other.type())
		{
			స్థితి DriveType::NONE:
				సమితి_none(1);
				ఆపు;
			స్థితి DriveType::CONSTANT:
				*this = Const(other.constant());
				ఆపు;
			స్థితి DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ఆపు;
			స్థితి DriveType::పోర్టు_2:
				*this = Drive_Chunk_పోర్టు(other.పోర్టు());
				ఆపు;
			స్థితి DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ఆపు;
			స్థితి DriveType::గుణిజం:
				*this = Drive_Chunk_గుణిజం(other.గుణిజం_2());
				ఆపు;
		}
		తిరిగి *this;
	}

	bool can_append(Drive_బిట్టు స్థిరరాశి &బిట్టు) స్థిరరాశి;
	bool try_append(Drive_బిట్టు స్థిరరాశి &బిట్టు);
	bool try_append(DriveChunk స్థిరరాశి &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

	bool క్రియాకారకం==(స్థిరరాశి DriveChunk &other) స్థిరరాశి
	{
		ఒకవేళ (type_ != other.type_)
			తిరిగి false;

		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి true;
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_ == other.constant_;
			స్థితి DriveType::WIRE:
				తిరిగి wire_ == other.wire_;
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3 == other.పోర్టు_3;
			స్థితి DriveType::MARKER:
				తిరిగి marker_ == other.marker_;
			స్థితి DriveType::గుణిజం:
				తిరిగి గుణిజం_3 == other.గుణిజం_3;
		}
		log_abort();
	}

	bool క్రియాకారకం!=(స్థిరరాశి DriveChunk &other) స్థిరరాశి
	{
		తిరిగి !(*this == other);
	}

	bool క్రియాకారకం<(స్థిరరాశి DriveChunk &other) స్థిరరాశి
	{
		ఒకవేళ (type_ != other.type_)
			తిరిగి type_ < other.type_;
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి false;
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_ < other.constant_;
			స్థితి DriveType::WIRE:
				తిరిగి wire_ < other.wire_;
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3 < other.పోర్టు_3;
			స్థితి DriveType::MARKER:
				తిరిగి marker_ < other.marker_;
			స్థితి DriveType::గుణిజం:
				తిరిగి గుణిజం_3 < other.గుణిజం_3;
		}
		log_abort();
	}

	DriveType type() స్థిరరాశి { తిరిగి type_; }

	bool is_none() స్థిరరాశి { తిరిగి type_ == DriveType::NONE; }
	bool is_constant() స్థిరరాశి { తిరిగి type_ == DriveType::CONSTANT; }
	bool is_wire() స్థిరరాశి { తిరిగి type_ == DriveType::WIRE; }
	bool is_పోర్టు() స్థిరరాశి { తిరిగి type_ == DriveType::పోర్టు_2; }
	bool is_marker() స్థిరరాశి { తిరిగి type_ == DriveType::MARKER; }
	bool is_గుణిజం() స్థిరరాశి { తిరిగి type_ == DriveType::గుణిజం; }

	Const &constant() { log_assert(is_constant()); తిరిగి constant_; }
	Const స్థిరరాశి &constant() స్థిరరాశి { log_assert(is_constant()); తిరిగి constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); తిరిగి wire_; }
	DriveChunkWire స్థిరరాశి &wire() స్థిరరాశి { log_assert(is_wire()); తిరిగి wire_; }
	Drive_Chunk_పోర్టు &పోర్టు() { log_assert(is_పోర్టు()); తిరిగి పోర్టు_3; }
	Drive_Chunk_పోర్టు స్థిరరాశి &పోర్టు() స్థిరరాశి { log_assert(is_పోర్టు()); తిరిగి పోర్టు_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); తిరిగి marker_; }
	DriveChunkMarker స్థిరరాశి &marker() స్థిరరాశి { log_assert(is_marker()); తిరిగి marker_; }
	Drive_Chunk_గుణిజం &గుణిజం_2() { log_assert(is_గుణిజం()); తిరిగి గుణిజం_3; }
	Drive_Chunk_గుణిజం స్థిరరాశి &గుణిజం_2() స్థిరరాశి { log_assert(is_గుణిజం()); తిరిగి గుణిజం_3; }


	పూర్ణాంకం కొలత() స్థిరరాశి
	{
		ఎంపిక (type_)
		{
			స్థితి DriveType::NONE:
				తిరిగి none_;
			స్థితి DriveType::CONSTANT:
				తిరిగి constant_.కొలత();
			స్థితి DriveType::WIRE:
				తిరిగి wire_.కొలత();
			స్థితి DriveType::పోర్టు_2:
				తిరిగి పోర్టు_3.కొలత();
			స్థితి DriveType::MARKER:
				తిరిగి marker_.కొలత();
			స్థితి DriveType::గుణిజం:
				తిరిగి గుణిజం_3.కొలత();
		}
		log_abort();
	}
};

అంతఃవరుస Drive_Chunk_గుణిజం::Drive_Chunk_గుణిజం(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు)
	: వెడల్పు_3(1)
{
	కొరకు (స్వయంచాలక స్థిరరాశి &బిట్టు : బిట్టు.గుణిజం_2())
		గుణిజం_3.emplace(బిట్టు);
}

నిర్మాణం DriveSpec
{
అంతరంగ:
	పూర్ణాంకం వెడల్పు_3 = 0;
	mutable std::సదిశ<DriveChunk> chunks_;
	mutable std::సదిశ<Drive_బిట్టు> బిట్లు_2;
	mutable గుర్తులేని పూర్ణాంకం hash_ = 0;
బహిరంగ:

	అంతఃవరుస bool packed() స్థిరరాశి {
		తిరిగి బిట్లు_2.ఖాళీలు();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk స్థిరరాశి &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire స్థిరరాశి &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_పోర్టు స్థిరరాశి &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker స్థిరరాశి &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_గుణిజం స్థిరరాశి &chunk) { *this = chunk; }

	DriveSpec(Drive_బిట్టు స్థిరరాశి &బిట్టు) { *this = బిట్టు; }
	DriveSpec(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు) { *this = బిట్టు; }
	DriveSpec(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు) { *this = బిట్టు; }
	DriveSpec(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు) { *this = బిట్టు; }
	DriveSpec(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు) { *this = బిట్టు; }

	DriveSpec(std::సదిశ<DriveChunk> స్థిరరాశి &chunks) : chunks_(chunks) { compute_వెడల్పు(); }

	DriveSpec(std::సదిశ<Drive_బిట్టు> స్థిరరాశి &బిట్లు)
	{
		కొరకు (స్వయంచాలక స్థిరరాశి &బిట్టు : బిట్లు)
			append(బిట్టు);
	}

	DriveSpec(SigSpec స్థిరరాశి &sig)
	{
		// TODO: converting one chunk at a time would be faster
		కొరకు (స్వయంచాలక స్థిరరాశి &బిట్టు : sig.బిట్లు())
			append(బిట్టు);
	}

	std::సదిశ<DriveChunk> స్థిరరాశి &chunks() స్థిరరాశి { pack(); తిరిగి chunks_; }
	std::సదిశ<Drive_బిట్టు> స్థిరరాశి &బిట్లు() స్థిరరాశి { unpack(); తిరిగి బిట్లు_2; }

	పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు_3; }

	శూన్య append(Drive_బిట్టు స్థిరరాశి &బిట్టు);

	శూన్య append(DriveChunk స్థిరరాశి &chunk);

	శూన్య pack() స్థిరరాశి;

	శూన్య unpack() స్థిరరాశి;

	Drive_బిట్టు &క్రియాకారకం[](పూర్ణాంకం సూచీ)
	{
		log_assert(సూచీ >= 0 && సూచీ < కొలత());
		unpack();
		తిరిగి బిట్లు_2[సూచీ];
	}

	స్థిరరాశి Drive_బిట్టు &క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి
	{
		log_assert(సూచీ >= 0 && సూచీ < కొలత());
		unpack();
		తిరిగి బిట్లు_2[సూచీ];
	}

	శూన్య clear()
	{
		chunks_.clear();
		బిట్లు_2.clear();
		వెడల్పు_3 = 0;
	}

	DriveSpec &క్రియాకారకం=(DriveChunk స్థిరరాశి &chunk)
	{
		chunks_.clear();
		బిట్లు_2.clear();
		append(chunk);
		తిరిగి *this;
	}

	DriveSpec &క్రియాకారకం=(DriveChunkWire స్థిరరాశి &chunk) { తిరిగి *this = DriveChunk(chunk); }
	DriveSpec &క్రియాకారకం=(Drive_Chunk_పోర్టు స్థిరరాశి &chunk) { తిరిగి *this = DriveChunk(chunk); }
	DriveSpec &క్రియాకారకం=(DriveChunkMarker స్థిరరాశి &chunk) { తిరిగి *this = DriveChunk(chunk); }
	DriveSpec &క్రియాకారకం=(Drive_Chunk_గుణిజం స్థిరరాశి &chunk) { తిరిగి *this = DriveChunk(chunk); }

	DriveSpec &క్రియాకారకం=(Drive_బిట్టు స్థిరరాశి &బిట్టు)
	{
		chunks_.clear();
		బిట్లు_2.clear();
		append(బిట్టు);
		తిరిగి *this;
	}

	DriveSpec &క్రియాకారకం=(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు) { తిరిగి *this = Drive_బిట్టు(బిట్టు); }
	DriveSpec &క్రియాకారకం=(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు) { తిరిగి *this = Drive_బిట్టు(బిట్టు); }
	DriveSpec &క్రియాకారకం=(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు) { తిరిగి *this = Drive_బిట్టు(బిట్టు); }
	DriveSpec &క్రియాకారకం=(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు) { తిరిగి *this = Drive_బిట్టు(బిట్టు); }

	శూన్య updhash() స్థిరరాశి {
		ఒకవేళ (hash_ != 0)
			తిరిగి;
		pack();
		hash_ = నిరంతరం_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;

	bool క్రియాకారకం==(DriveSpec స్థిరరాశి &other) స్థిరరాశి {
		updhash();
		other.updhash();
		ఒకవేళ (కొలత() != other.కొలత() || hash_ != other.hash_)
			తిరిగి false;
		తిరిగి chunks() == other.chunks();
	}

అంతరంగ:
	శూన్య compute_వెడల్పు();
};



నిర్మాణం DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.అమరిక(); }
	DriverMap(Design *design) { celltypes.అమరిక(design); }

అంతరంగ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	నిర్మాణం Drive_బిట్టు_Id
	{
		పూర్ణాంకం id = -1;

		Drive_బిట్టు_Id() {};

		Drive_బిట్టు_Id(పూర్ణాంకం id) : id(id) { }

		bool క్రియాకారకం==(స్థిరరాశి Drive_బిట్టు_Id &other) స్థిరరాశి { తిరిగి id == other.id; }
		bool క్రియాకారకం!=(స్థిరరాశి Drive_బిట్టు_Id &other) స్థిరరాశి { తిరిగి id != other.id; }
		bool క్రియాకారకం<(స్థిరరాశి Drive_బిట్టు_Id &other) స్థిరరాశి { తిరిగి id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	నిర్మాణం Drive_బిట్టు_లేఖాచిత్రం {
		dict<Drive_బిట్టు_Id, Drive_బిట్టు_Id> మొదటి_అంచులు;
		dict<Drive_బిట్టు_Id, Drive_బిట్టు_Id> రెండవ_అంచులు;
		dict<Drive_బిట్టు_Id, pool<Drive_బిట్టు_Id>> మరిన్ని_అంచులు;

		శూన్య కూడు_అంచు(Drive_బిట్టు_Id src, Drive_బిట్టు_Id dst);
		Drive_బిట్టు_Id తీయి_అంచు(Drive_బిట్టు_Id src);
		శూన్య clear(Drive_బిట్టు_Id src);
		bool కలిగియుంది(Drive_బిట్టు_Id src);
		పూర్ణాంకం లెక్క(Drive_బిట్టు_Id src);

		Drive_బిట్టు_Id at(Drive_బిట్టు_Id src, పూర్ణాంకం సూచీ);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_బిట్టు_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_బిట్టు_Id> పోర్టు_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_బిట్టు_Id, Drive_బిట్టు> drive_బిట్లు;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_బిట్టు_Id, Drive_బిట్టు> isolated_drive_బిట్లు;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	పూర్ణాంకం తదుపరి_స్థానభ్రంశం = 1 + (పూర్ణాంకం)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_బిట్టు_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_బిట్టు_లేఖాచిత్రం connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_బిట్టు_లేఖాచిత్రం connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_బిట్టు_లేఖాచిత్రం connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_బిట్టు_Id> oriented_present;


	గణన class బిట్టు_బహుళకం {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	బిట్టు_బహుళకం బిట్టు_బహుళకం_2(Drive_బిట్టు స్థిరరాశి &బిట్టు);
	Drive_బిట్టు_Id id_from_drive_బిట్టు(Drive_బిట్టు స్థిరరాశి &బిట్టు);
	Drive_బిట్టు drive_బిట్టు_from_id(Drive_బిట్టు_Id id);

	శూన్య connect_directed_విలీనం(Drive_బిట్టు_Id driven_id, Drive_బిట్టు_Id driver_id);
	శూన్య connect_directed_బఫరు(Drive_బిట్టు_Id driven_id, Drive_బిట్టు_Id driver_id);
	శూన్య connect_undirected(Drive_బిట్టు_Id a_id, Drive_బిట్టు_Id b_id);

బహిరంగ:

	శూన్య కూడు(Module *module);

	// Add a single bit connection to the driver map.
	శూన్య కూడు(Drive_బిట్టు స్థిరరాశి &a, Drive_బిట్టు స్థిరరాశి &b);

	మూస<typename T>
	స్థిర constexpr bool is_sig_type() {
		తిరిగి
			std::is_same<T, SigSpec>::విలువ ||
			std::is_same<T, SigChunk>::విలువ ||
			std::is_same<T, DriveSpec>::విలువ ||
			std::is_same<T, DriveChunk>::విలువ ||
			std::is_same<T, Drive_Chunk_పోర్టు>::విలువ ||
			std::is_same<T, DriveChunkWire>::విలువ ||
			std::is_same<T, Const>::విలువ;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	మూస<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	కూడు(T స్థిరరాశి &a, U స్థిరరాశి &b)
	{
		log_assert(a.కొలత() == b.కొలత());
		కొరకు (పూర్ణాంకం i = 0; i != Get_కొలత(a); ++i)
			కూడు(Drive_బిట్టు(a[i]), Drive_బిట్టు(b[i]));
	}


	// Specialized version that avoids unpacking
	శూన్య కూడు(SigSpec స్థిరరాశి &a, SigSpec స్థిరరాశి &b);

అంతరంగ:
	శూన్య కూడు_పోర్టు(Cell *cell, IdString పోర్టు, SigSpec స్థిరరాశి &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_బిట్టు_Id> orient_undirected_చూసిన;
	pool<Drive_బిట్టు_Id> orient_undirected_drivers;
	dict<Drive_బిట్టు_Id, పూర్ణాంకం> orient_undirected_దూరం;

	శూన్య orient_undirected(Drive_బిట్టు_Id id);

బహిరంగ:
	Drive_బిట్టు క్రియాకారకం()(Drive_బిట్టు స్థిరరాశి &బిట్టు);

	DriveSpec క్రియాకారకం()(DriveSpec spec);

అంతరంగ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		తిరిగి wire->has_attribute(ID(keep));
	}
};

అంతఃవరుస Hasher Drive_బిట్టు_Wire::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(wire->name);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_బిట్టు_పోర్టు::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(cell->name);
	h.eat(పోర్టు);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_బిట్టు_Marker::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(marker);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_బిట్టు_గుణిజం::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(గుణిజం_3);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_బిట్టు::hash_into(Hasher h) స్థిరరాశి
{
	ఎంపిక (type_) {
	స్థితి DriveType::NONE:
		h.eat(0);
		ఆపు;
	స్థితి DriveType::CONSTANT:
		h.eat(constant_);
		ఆపు;
	స్థితి DriveType::WIRE:
		h.eat(wire_);
		ఆపు;
	స్థితి DriveType::పోర్టు_2:
		h.eat(పోర్టు_3);
		ఆపు;
	స్థితి DriveType::MARKER:
		h.eat(marker_);
		ఆపు;
	స్థితి DriveType::గుణిజం:
		h.eat(గుణిజం_3);
		ఆపు;
	}
	h.eat(type_);
	తిరిగి h;
}

అంతఃవరుస Hasher DriveChunkWire::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(wire->name);
	h.eat(వెడల్పు);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_Chunk_పోర్టు::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(cell->name);
	h.eat(పోర్టు);
	h.eat(వెడల్పు);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher DriveChunkMarker::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(marker);
	h.eat(వెడల్పు);
	h.eat(స్థానభ్రంశం);
	తిరిగి h;
}

అంతఃవరుస Hasher Drive_Chunk_గుణిజం::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(వెడల్పు_3);
	h.eat(గుణిజం_3);
	తిరిగి h;
}

అంతఃవరుస Hasher DriveChunk::hash_into(Hasher h) స్థిరరాశి
{
	ఎంపిక (type_) {
	స్థితి DriveType::NONE:
		h.eat(0);
		ఆపు;
	స్థితి DriveType::CONSTANT:
		h.eat(constant_);
		ఆపు;
	స్థితి DriveType::WIRE:
		h.eat(wire_);
		ఆపు;
	స్థితి DriveType::పోర్టు_2:
		h.eat(పోర్టు_3);
		ఆపు;
	స్థితి DriveType::MARKER:
		h.eat(marker_);
		ఆపు;
	స్థితి DriveType::గుణిజం:
		h.eat(గుణిజం_3);
		ఆపు;
	}
	h.eat(type_);
	తిరిగి h;
}

అంతఃవరుస Hasher DriveSpec::hash_into(Hasher h) స్థిరరాశి
{
	updhash();
	h.eat(hash_);
	తిరిగి h;
}

అంతఃవరుస Hasher DriverMap::Drive_బిట్టు_Id::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(id);
	తిరిగి h;
}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
