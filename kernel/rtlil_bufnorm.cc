/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/modtools.h"

#अन्तर्भाव <string.h>
#अन्तर्भाव <algorithm>
#अन्तर्भाव <optional>

YOSYS_NAMESPACE_BEGIN


स्थिर शून्य buf_norm_बीजम्_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	प्रति (स्वचालित cell : module->cells())
	प्रति (स्वचालित &conn : cell->connections()) {
		यदि (Get_आकार(conn.second) == 0 || (cell->पत्तनम्_dir(conn.first) != RTLIL::PD_OUTPUT && cell->पत्तनम्_dir(conn.first) != RTLIL::PD_INOUT))
			अनुवर्त;
		module->buf_norm_cell_queue.निवेशय(cell);
		module->buf_norm_cell_पत्तनम्_queue.emplace(cell, conn.first);
	}
	प्रति (स्वचालित wire : module->wires())
		module->buf_norm_wire_queue.निवेशय(wire);
}

शून्य RTLIL::Design::bufNormalize(तार्किक enable)
{
	यदि (!enable)
	{
		यदि (!flagBufferedNormalized)
			प्रत्यागम;

		प्रति (स्वचालित module : modules()) {
			module->buf_norm_cell_queue.clear();
			module->buf_norm_wire_queue.clear();
			module->buf_norm_cell_पत्तनम्_queue.clear();
			प्रति (स्वचालित wire : module->wires()) {
				wire->driverCell_ = शून्यसूचकः;
				wire->driver_पत्तनम्_2 = IdString();
			}
			module->buf_norm_connect_सूचकांक.clear();
		}

		flagBufferedNormalized = असत्य;
		प्रत्यागम;
	}

	यदि (!flagBufferedNormalized)
	{
		प्रति (स्वचालित module : modules())
			buf_norm_बीजम्_queues(module);

		flagBufferedNormalized = सत्य;
	}

	प्रति (स्वचालित module : modules())
		module->bufNormalize();
}

संरचना bit_drive_दत्त_t {
	पूर्णाङ्क drivers = 0;
	पूर्णाङ्क inout = 0;
	पूर्णाङ्क users = 0;
};

प्रकारनिर्देश ModWalker::पत्तनम्_Bit पत्तनम्_Bit;

शून्य RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#परिभाषय xlog(...) कुरु { यदि (yosys_xtrace) लघुगणकः("#X [bufnorm] " __VA_ARGS__); } यावत् (0)

	यदि (!design->flagBufferedNormalized)
		प्रत्यागम;

	यदि (!buf_norm_cell_queue.रिक्तानि() || !buf_norm_wire_queue.रिक्तानि() || !connections_.रिक्तानि())
	{
		// Ensure that every enqueued input port is represented by a cell
		प्रति (स्वचालित wire : buf_norm_wire_queue) {
			यदि (wire->पत्तनम्_निवेशः && !wire->पत्तनम्_output && Get_आकार(wire) != 0) {
				यदि (wire->driverCell_ != शून्यसूचकः && wire->driverCell_->type != ID($निवेशः_पत्तनम्)) {
					wire->driverCell_ = शून्यसूचकः;
					wire->driver_पत्तनम्_2 = IdString::Null;
				}
				यदि (wire->driverCell_ == शून्यसूचकः) {
					Cell *निवेशः_पत्तनम्_cell = सङ्कलय_Cell(NEW_ID, ID($निवेशः_पत्तनम्));
					निवेशः_पत्तनम्_cell->समुच्चयः_Param(ID::विस्तारः_2, Get_आकार(wire));
					निवेशः_पत्तनम्_cell->समुच्चयः_पत्तनम्(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
				}
			}
		}

		// Next we will temporarily undo buf normalization locally for
		// everything enqueued. This means we will turn $buf and $connect back
		// into connections. When doing this we also need to enqueue the other
		// end of $buf and $connect cells, so we use a queue and do this until
		// reaching a fixed point.

		// While doing this, we will also discover all drivers fully connected
		// to enqueued wires. We keep track of which wires are driven by a
		// unique and full cell ports (in which case the wire can stay
		// connected to the port) and which cell ports will need to be
		// reconnected to a fresh intermediate wire to re-normalize the module.

		idict<Wire *> wire_queue_entries; // Ordered queue of wires to process
		पूर्णाङ्क wire_queue_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> प्रत्यक्षम्_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		स्वचालित पङ्क्तौ_निक्षिप_cell_पत्तनम् = [&](Cell *cell, IdString पत्तनम्) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(पत्तनम्));

			// An empty cell type means the cell got removed
			यदि (cell->type.रिक्तानि())
				प्रत्यागम;


			SigSpec ध्रुव &sig = cell->get_पत्तनम्(पत्तनम्);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			प्रति (स्वचालित chunk : sig.chunks())
				यदि (chunk.is_wire())
					wire_queue_entries(chunk.wire);

			यदि (cell->type == ID($buf) && cell->गुणाः.रिक्तानि() && !cell->name.is_लौकिकम्()) {
				// For a plain `$buf` cell, we enqueue all wires on its input
				// side, bypass it using module level connections (skipping 'z
				// bits) and then remove the cell. Eventually the module level
				// connections will turn back into `$buf` and `$connect` cells,
				// but since we also need to handle externally added module
				// level connections, turning everything into connections first
				// simplifies the logic for doing so.

				// TODO: We could defer removing the $buf cells here, and
				// re-use them in case we would create a new identical cell
				// later.
				log_assert(पत्तनम् == ID::Y);
				SigSpec sig_a = cell->get_पत्तनम्(ID::A);
				SigSpec sig_y = sig;

				प्रति (स्वचालित ध्रुव &s : {sig_a, sig})
					प्रति (स्वचालित ध्रुव &chunk : s.chunks())
						यदि (chunk.wire)
							wire_queue_entries(chunk.wire);

				यदि (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig_a); ++i) {
						SigBit b = sig_a[i];
						यदि (b == State::Sz)
							अनुवर्त;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				यदि (!sig_y.रिक्तानि())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_आकार(buf_norm_wire_queue) <= 1);
				buf_norm_wire_queue.clear();
				प्रत्यागम;
			} अन्यथा यदि (cell->type == ID($निवेशः_पत्तनम्)) {
				log_assert(पत्तनम् == ID::Y);
				यदि (sig.is_wire()) {
					Wire *w = sig.as_wire();
					यदि (w->पत्तनम्_निवेशः && !w->पत्तनम्_output) {
						// An $input_port cell can only drive a full wire module input port
						स्वचालित [प्राप्तम्, inserted] = प्रत्यक्षम्_driven_wires.emplace(w, {cell, पत्तनम्});
						यदि (!inserted || (प्राप्तम्->second.first == cell && प्राप्तम्->second.second == पत्तनम्))
							प्रत्यागम;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_queue.निवेशय(cell);
				remove(cell);
				log_assert(Get_आकार(buf_norm_wire_queue) <= 1);
				buf_norm_wire_queue.clear();
				प्रत्यागम;
			}

			यदि (sig.is_wire()) {
				Wire *w = sig.as_wire();
				यदि (!w->पत्तनम्_निवेशः || w->पत्तनम्_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					स्वचालित [प्राप्तम्, inserted] = प्रत्यक्षम्_driven_wires.emplace(w, {cell, पत्तनम्});
					यदि (inserted || (प्राप्तम्->second.first == cell && प्राप्तम्->second.second == पत्तनम्))
						प्रत्यागम;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, पत्तनम्);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		प्रति (स्वचालित wire : buf_norm_wire_queue)
			wire_queue_entries(wire);
		buf_norm_wire_queue.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		प्रति (स्वचालित ध्रुव &[cell, पत्तनम्_name] : buf_norm_cell_पत्तनम्_queue)
			पङ्क्तौ_निक्षिप_cell_पत्तनम्(cell, पत्तनम्_name);
		buf_norm_cell_पत्तनम्_queue.clear();

		// We also enqueue all wires that saw newly added module level connections.
		प्रति (स्वचालित &[a, b] : connections_)
			प्रति (स्वचालित &sig : {a, b})
				प्रति (स्वचालित ध्रुव &chunk : sig.chunks())
					यदि (chunk.wire)
						wire_queue_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		यावत् (wire_queue_pos < Get_आकार(wire_queue_entries)) {
			स्वचालित wire = wire_queue_entries[wire_queue_pos++];
			xlog("processing wire %s\n", wire);

			यदि (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString पत्तनम् = wire->driver_पत्तनम्_2;
				पङ्क्तौ_निक्षिप_cell_पत्तनम्(cell, पत्तनम्);
			}

			यावत् (सत्य) {
				स्वचालित प्राप्तम् = buf_norm_connect_सूचकांक.find(wire);
				यदि (प्राप्तम् == buf_norm_connect_सूचकांक.अन्तः())
					भङ्ग;
				यावत् (!प्राप्तम्->second.रिक्तानि()) {
					Cell *connect_cell = *प्राप्तम्->second.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ध्रुव &sig_a = connect_cell->get_पत्तनम्(ID::A);
					SigSpec ध्रुव &sig_b = connect_cell->get_पत्तनम्(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					प्रति (स्वचालित &side : {sig_a, sig_b})
						प्रति (स्वचालित chunk : side.chunks())
							यदि (chunk.wire)
								wire_queue_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_queue.निवेशय(connect_cell);
					remove(connect_cell);
					log_assert(Get_आकार(buf_norm_wire_queue) <= 2);
					buf_norm_wire_queue.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		प्रति (स्वचालित &[cell, पत्तनम्] : pending_ports) {
			log_assert(cell->type != ID($निवेशः_पत्तनम्));
			log_assert(!cell->type.रिक्तानि());
			log_assert(!pending_deleted_cells.गणनम्(cell));
			SigSpec ध्रुव &sig = cell->get_पत्तनम्(पत्तनम्);
			Wire *w = सङ्कलय_Wire(NEW_ID, Get_आकार(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			प्रत्यक्षम्_driven_wires.emplace(w, {cell, पत्तनम्});
			cell->समुच्चयः_पत्तनम्(पत्तनम्, w); // Hits the fast path that doesn't enqueue w
			wire_queue_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		प्रति (स्वचालित wire : wire_queue_entries) {
			wire->driverCell_ = शून्यसूचकः;
			wire->driver_पत्तनम्_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		प्रति (स्वचालित ध्रुव &[wire, cellport] : प्रत्यक्षम्_driven_wires) {
			wire->driverCell_ = cellport.first;
			wire->driver_पत्तनम्_2 = cellport.second;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		यदि (yosys_xtrace)
			प्रति (स्वचालित ध्रुव &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(एतत्);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		प्रति (तार्किक निवेशः_ports : {असत्य, सत्य}) {
			प्रति (स्वचालित ध्रुव &[wire, cellport] : प्रत्यक्षम्_driven_wires) {
				यदि ((wire->पत्तनम्_निवेशः && !wire->पत्तनम्_output) == निवेशः_ports) {
					प्रति (पूर्णाङ्क i = 0; i != Get_आकार(wire); ++i) {
						SigBit driver = SigBit(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<SigBit> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<SigBit> conflicted;
		// Bits that are driven by an inout driver
		pool<SigBit> weakly_driven;

		प्रति (स्वचालित ध्रुव &[wire, cellport] : प्रत्यक्षम्_driven_wires) {
			स्वचालित ध्रुव &[cell, पत्तनम्] = cellport;
			प्रति (पूर्णाङ्क i = 0; i != Get_आकार(wire); ++i) {
				SigBit driver = sigmap(SigBit(wire, i));
				यदि (cell->type == ID($tribuf) || cell->पत्तनम्_dir(पत्तनम्) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.निवेशय(driver);
				} अन्यथा {
					// We remove driver conflicts from `driven` in a separate loop below
					तार्किक inserted = driven.निवेशय(driver).second;
					यदि (!inserted)
						conflicted.निवेशय(driver);
				}
			}
		}

		प्रति (स्वचालित driver : weakly_driven)
			driven.निवेशय(driver);

		// This only leaves the drivers matching `driven`'s definition above
		प्रति (स्वचालित driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<SigBit, SigBit>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		प्रति (स्वचालित wire : wire_queue_entries) {
			// A zero-width wire carries no bits and needs no driver
			यदि (Get_आकार(wire) == 0)
				अनुवर्त;

			SigSpec wire_drivers;
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wire); ++i) {
				SigBit bit(wire, i);
				SigBit mapped = sigmap(bit);
				xlog("bit %s -> mapped %s\n", log_signal(bit), log_signal(mapped));


				buf_connected.apply(bit);
				buf_connected.सङ्कलय(bit, mapped);
				buf_connected.database.promote(mapped);

				यदि (wire->driverCell_ == शून्यसूचकः) {
					यदि (!mapped.is_wire() || driven.गणनम्(mapped)) {
						wire_drivers.append(mapped);
						अनुवर्त;
					} अन्यथा {
						wire_drivers.append(State::Sz);
					}
				}

				यदि (bit < mapped)
					undirected_connections.emplace(bit, mapped);
				अन्यथा यदि (mapped < bit)
					undirected_connections.emplace(mapped, bit);
			}

			यदि (wire->driverCell_ == शून्यसूचकः) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				सङ्कलय_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		स्थिर स्वचालित क्रमय_कुञ्चिका = [](std::pair<SigBit, SigBit> ध्रुव &p) {
			पूर्णाङ्क first_विस्थापनम् = p.first.is_wire() ? p.first.विस्थापनम् : 0;
			पूर्णाङ्क second_विस्थापनम् = p.second.is_wire() ? p.second.विस्थापनम् : 0;
			प्रत्यागम std::make_tuple(p.first.wire, p.second.wire, first_विस्थापनम् - second_विस्थापनम्, p);
		};

		undirected_connections.क्रमय([](std::pair<SigBit, SigBit> ध्रुव &p, std::pair<SigBit, SigBit> ध्रुव &q) {
			प्रत्यागम क्रमय_कुञ्चिका(p) < क्रमय_कुञ्चिका(q);
		});

		SigSpec tmp_a, tmp_b;

		प्रति (स्वचालित &[bit_a, bit_b] : undirected_connections) {
			tmp_a.append(bit_a);
			tmp_b.append(bit_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		SigBit अग्र_a, अग्र_b;

		स्वचालित emit_connect_cell = [&]() {
			यदि (sig_a.रिक्तानि())
				प्रत्यागम;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = सङ्कलय_Cell(NEW_ID, ID($connect));
			connect_cell->समुच्चयः_Param(ID::विस्तारः_2, Get_आकार(sig_a));
			connect_cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
			connect_cell->समुच्चयः_पत्तनम्(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		प्रति (स्वचालित &[bit_a, bit_b] : undirected_connections) {
			यदि (bit_a == bit_b)
				अनुवर्त;
			यदि (bit_a != अग्र_a || bit_b != अग्र_b)
				emit_connect_cell();

			sig_a.append(bit_a);
			sig_b.append(bit_b);
			अग्र_a = bit_a;
			अग्र_b = bit_b;
			यदि (अग्र_a.is_wire())
				अग्र_a.विस्थापनम्++;
			यदि (अग्र_b.is_wire())
				अग्र_b.विस्थापनम्++;

		}
		emit_connect_cell();

		buf_norm_cell_queue.clear();

		log_assert(buf_norm_cell_पत्तनम्_queue.रिक्तानि());
		log_assert(buf_norm_wire_queue.रिक्तानि());
		log_assert(connections_.रिक्तानि());
	}

	प्रति (स्वचालित cell : pending_deleted_cells) {
		विलोपय cell;
	}
	pending_deleted_cells.clear();
}

शून्य RTLIL::Cell::unset_पत्तनम्(RTLIL::IdString portname)
{
	RTLIL::SigSpec सङ्केत;
	स्वचालित conn_it = connections_.find(portname);

	यदि (conn_it != connections_.अन्तः())
	{
		प्रति (स्वचालित mon : module->monitors)
			mon->notify_connect(एतत्, conn_it->first, conn_it->second, सङ्केत);

		यदि (module->design)
			प्रति (स्वचालित mon : module->design->monitors)
				mon->notify_connect(एतत्, conn_it->first, conn_it->second, सङ्केत);

		यदि (yosys_xtrace) {
			लघुगणकः("#X# Unconnect %s.%s.%s\n", एतत्->module, एतत्, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		यदि (module->design && module->design->flagBufferedNormalized) {
			यदि (conn_it->second.is_wire()) {
				Wire *w = conn_it->second.as_wire();
				यदि (w->driverCell_ == एतत् && w->driver_पत्तनम्_2 == portname) {
					w->driverCell_ = शून्यसूचकः;
					w->driver_पत्तनम्_2 = IdString();
					module->buf_norm_wire_queue.निवेशय(w);
				} अन्यथा यदि (w->driverCell_) {
					log_assert(w->driverCell_->get_पत्तनम्(w->driver_पत्तनम्_2) == w);
				}
			}

			यदि (type == ID($connect)) {
				प्रति (स्वचालित &[पत्तनम्, sig] : connections_) {
					प्रति (स्वचालित &chunk : sig.chunks()) {
						यदि (!chunk.wire)
							अनुवर्त;
						स्वचालित it = module->buf_norm_connect_सूचकांक.find(chunk.wire);
						यदि (it == module->buf_norm_connect_सूचकांक.अन्तः())
							अनुवर्त;
						it->second.erase(एतत्);
						यदि (it->second.रिक्तानि())
							module->buf_norm_connect_सूचकांक.erase(it);
					}
				}
				connections_.erase(conn_it);
				प्रति (स्वचालित &[पत्तनम्, sig] : connections_) {
					प्रति (स्वचालित &chunk : sig.chunks()) {
						यदि (!chunk.wire)
							अनुवर्त;
						module->buf_norm_connect_सूचकांक[chunk.wire].निवेशय(एतत्);
					}
				}
				प्रत्यागम;
			}
		}

		connections_.erase(conn_it);
	}
}

शून्य RTLIL::Cell::समुच्चयः_पत्तनम्(RTLIL::IdString portname, RTLIL::SigSpec सङ्केत)
{
	स्वचालित r = connections_.निवेशय(portname);
	स्वचालित conn_it = r.first;
	यदि (!r.second && conn_it->second == सङ्केत)
		प्रत्यागम;

	प्रति (स्वचालित mon : module->monitors)
		mon->notify_connect(एतत्, conn_it->first, conn_it->second, सङ्केत);

	यदि (module->design)
		प्रति (स्वचालित mon : module->design->monitors)
			mon->notify_connect(एतत्, conn_it->first, conn_it->second, सङ्केत);

	यदि (yosys_xtrace) {
		लघुगणकः("#X# Connect %s.%s.%s = %s (%d)\n", एतत्->module, एतत्, PooledName(module, portname).unescape(), log_signal(सङ्केत), Get_आकार(सङ्केत));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	यदि (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		यदि (conn_it->second.is_wire()) {
			Wire *w = conn_it->second.as_wire();
			यदि (w->driverCell_ == एतत् && w->driver_पत्तनम्_2 == portname) {
				w->driverCell_ = शून्यसूचकः;
				w->driver_पत्तनम्_2 = IdString();
				module->buf_norm_wire_queue.निवेशय(w);
			}
		}

		स्वचालित dir = पत्तनम्_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		यदि ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && सङ्केत.is_wire()) {
			Wire *w = सङ्केत.as_wire();
			यदि (w->driverCell_ == शून्यसूचकः && (
						(w->पत्तनम्_निवेशः && !w->पत्तनम्_output) == (type == ID($निवेशः_पत्तनम्)))) {
				w->driverCell_ = एतत्;
				w->driver_पत्तनम्_2 = portname;

				conn_it->second = std::move(सङ्केत);
				प्रत्यागम;
			}
		}

		यदि (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_queue.निवेशय(एतत्);
			module->buf_norm_cell_पत्तनम्_queue.emplace(एतत्, portname);
		} अन्यथा {
			प्रति (स्वचालित &chunk : सङ्केत.chunks())
				यदि (chunk.wire != शून्यसूचकः && chunk.wire->driverCell_ == शून्यसूचकः)
					module->buf_norm_wire_queue.निवेशय(chunk.wire);
		}

		यदि (type == ID($connect)) {
			प्रति (स्वचालित &[पत्तनम्, sig] : connections_) {
				प्रति (स्वचालित &chunk : sig.chunks()) {
					यदि (!chunk.wire)
						अनुवर्त;
					स्वचालित it = module->buf_norm_connect_सूचकांक.find(chunk.wire);
					यदि (it == module->buf_norm_connect_सूचकांक.अन्तः())
						अनुवर्त;
					it->second.erase(एतत्);
					यदि (it->second.रिक्तानि())
						module->buf_norm_connect_सूचकांक.erase(it);
				}
			}
			conn_it->second = std::move(सङ्केत);
			प्रति (स्वचालित &[पत्तनम्, sig] : connections_) {
				प्रति (स्वचालित &chunk : sig.chunks()) {
					यदि (!chunk.wire)
						अनुवर्त;
					module->buf_norm_connect_सूचकांक[chunk.wire].निवेशय(एतत्);
				}
			}
			प्रत्यागम;
		}
	}
	conn_it->second = std::move(सङ्केत);

}

शून्य RTLIL::Design::सङ्कलय(RTLIL::Module *module)
{
	log_assert(modules_.गणनम्(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = एतत्;

	प्रति (स्वचालित mon : monitors)
		mon->notify_module_सङ्कलय(module);

	यदि (yosys_xtrace) {
		लघुगणकः("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	यदि (flagBufferedNormalized) {
		buf_norm_बीजम्_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_अन्तः
