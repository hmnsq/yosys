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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/modtools.h"

#అన్తర్భావ <string.h>
#అన్తర్భావ <algorithm>
#అన్తర్భావ <optional>

YOSYS_NAMESPACE_BEGIN


స్థిర శూన్య buf_norm_విత్తనం_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	కొరకు (స్వయంచాలక cell : module->cells())
	కొరకు (స్వయంచాలక &conn : cell->connections()) {
		ఒకవేళ (Get_కొలత(conn.రెండవ) == 0 || (cell->పోర్టు_dir(conn.మొదటి) != RTLIL::PD_OUTPUT && cell->పోర్టు_dir(conn.మొదటి) != RTLIL::PD_INOUT))
			కొనసాగు;
		module->buf_norm_cell_వరుస.చొప్పించు(cell);
		module->buf_norm_cell_పోర్టు_వరుస.emplace(cell, conn.మొదటి);
	}
	కొరకు (స్వయంచాలక wire : module->wires())
		module->buf_norm_wire_వరుస.చొప్పించు(wire);
}

శూన్య RTLIL::Design::bufNormalize(తార్కిక enable)
{
	ఒకవేళ (!enable)
	{
		ఒకవేళ (!flagBufferedNormalized)
			తిరిగి;

		కొరకు (స్వయంచాలక module : modules()) {
			module->buf_norm_cell_వరుస.clear();
			module->buf_norm_wire_వరుస.clear();
			module->buf_norm_cell_పోర్టు_వరుస.clear();
			కొరకు (స్వయంచాలక wire : module->wires()) {
				wire->driverCell_ = శూన్యసూచిక;
				wire->driver_పోర్టు_2 = IdString();
			}
			module->buf_norm_connect_సూచీ.clear();
		}

		flagBufferedNormalized = అబద్ధం;
		తిరిగి;
	}

	ఒకవేళ (!flagBufferedNormalized)
	{
		కొరకు (స్వయంచాలక module : modules())
			buf_norm_విత్తనం_queues(module);

		flagBufferedNormalized = నిజం;
	}

	కొరకు (స్వయంచాలక module : modules())
		module->bufNormalize();
}

నిర్మాణం బిట్టు_drive_దత్తాంశం_t {
	పూర్ణాంకం drivers = 0;
	పూర్ణాంకం inout = 0;
	పూర్ణాంకం users = 0;
};

రకనిర్వచనం ModWalker::పోర్టు_బిట్టు_2 పోర్టు_బిట్టు_2;

శూన్య RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#నిర్వచించు xlog(...) చేయి { ఒకవేళ (yosys_xtrace) సంవర్గమానం("#X [bufnorm] " __VA_ARGS__); } అయితే (0)

	ఒకవేళ (!design->flagBufferedNormalized)
		తిరిగి;

	ఒకవేళ (!buf_norm_cell_వరుస.ఖాళీలు() || !buf_norm_wire_వరుస.ఖాళీలు() || !connections_.ఖాళీలు())
	{
		// Ensure that every enqueued input port is represented by a cell
		కొరకు (స్వయంచాలక wire : buf_norm_wire_వరుస) {
			ఒకవేళ (wire->పోర్టు_ఆదానం && !wire->పోర్టు_output && Get_కొలత(wire) != 0) {
				ఒకవేళ (wire->driverCell_ != శూన్యసూచిక && wire->driverCell_->type != ID($ఆదానం_పోర్టు)) {
					wire->driverCell_ = శూన్యసూచిక;
					wire->driver_పోర్టు_2 = IdString::Null;
				}
				ఒకవేళ (wire->driverCell_ == శూన్యసూచిక) {
					Cell *ఆదానం_పోర్టు_cell = కూడు_Cell(NEW_ID, ID($ఆదానం_పోర్టు));
					ఆదానం_పోర్టు_cell->సమితి_Param(ID::వెడల్పు_2, Get_కొలత(wire));
					ఆదానం_పోర్టు_cell->సమితి_పోర్టు(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
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

		idict<Wire *> wire_వరుస_entries; // Ordered queue of wires to process
		పూర్ణాంకం wire_వరుస_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> ప్రత్యక్ష_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		స్వయంచాలక వరుసలో_చేర్చు_cell_పోర్టు = [&](Cell *cell, IdString పోర్టు) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(పోర్టు));

			// An empty cell type means the cell got removed
			ఒకవేళ (cell->type.ఖాళీలు())
				తిరిగి;


			SigSpec స్థిరరాశి &sig = cell->get_పోర్టు(పోర్టు);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			కొరకు (స్వయంచాలక chunk : sig.chunks())
				ఒకవేళ (chunk.is_wire())
					wire_వరుస_entries(chunk.wire);

			ఒకవేళ (cell->type == ID($buf) && cell->లక్షణాలు.ఖాళీలు() && !cell->name.is_ప్రజా()) {
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
				log_assert(పోర్టు == ID::Y);
				SigSpec sig_a = cell->get_పోర్టు(ID::A);
				SigSpec sig_y = sig;

				కొరకు (స్వయంచాలక స్థిరరాశి &s : {sig_a, sig})
					కొరకు (స్వయంచాలక స్థిరరాశి &chunk : s.chunks())
						ఒకవేళ (chunk.wire)
							wire_వరుస_entries(chunk.wire);

				ఒకవేళ (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig_a); ++i) {
						Sig_బిట్టు b = sig_a[i];
						ఒకవేళ (b == State::Sz)
							కొనసాగు;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				ఒకవేళ (!sig_y.ఖాళీలు())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_కొలత(buf_norm_wire_వరుస) <= 1);
				buf_norm_wire_వరుస.clear();
				తిరిగి;
			} లేకపోతే ఒకవేళ (cell->type == ID($ఆదానం_పోర్టు)) {
				log_assert(పోర్టు == ID::Y);
				ఒకవేళ (sig.is_wire()) {
					Wire *w = sig.as_wire();
					ఒకవేళ (w->పోర్టు_ఆదానం && !w->పోర్టు_output) {
						// An $input_port cell can only drive a full wire module input port
						స్వయంచాలక [దొరికింది, inserted] = ప్రత్యక్ష_driven_wires.emplace(w, {cell, పోర్టు});
						ఒకవేళ (!inserted || (దొరికింది->రెండవ.మొదటి == cell && దొరికింది->రెండవ.రెండవ == పోర్టు))
							తిరిగి;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_వరుస.చొప్పించు(cell);
				remove(cell);
				log_assert(Get_కొలత(buf_norm_wire_వరుస) <= 1);
				buf_norm_wire_వరుస.clear();
				తిరిగి;
			}

			ఒకవేళ (sig.is_wire()) {
				Wire *w = sig.as_wire();
				ఒకవేళ (!w->పోర్టు_ఆదానం || w->పోర్టు_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					స్వయంచాలక [దొరికింది, inserted] = ప్రత్యక్ష_driven_wires.emplace(w, {cell, పోర్టు});
					ఒకవేళ (inserted || (దొరికింది->రెండవ.మొదటి == cell && దొరికింది->రెండవ.రెండవ == పోర్టు))
						తిరిగి;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, పోర్టు);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		కొరకు (స్వయంచాలక wire : buf_norm_wire_వరుస)
			wire_వరుస_entries(wire);
		buf_norm_wire_వరుస.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		కొరకు (స్వయంచాలక స్థిరరాశి &[cell, పోర్టు_name] : buf_norm_cell_పోర్టు_వరుస)
			వరుసలో_చేర్చు_cell_పోర్టు(cell, పోర్టు_name);
		buf_norm_cell_పోర్టు_వరుస.clear();

		// We also enqueue all wires that saw newly added module level connections.
		కొరకు (స్వయంచాలక &[a, b] : connections_)
			కొరకు (స్వయంచాలక &sig : {a, b})
				కొరకు (స్వయంచాలక స్థిరరాశి &chunk : sig.chunks())
					ఒకవేళ (chunk.wire)
						wire_వరుస_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		అయితే (wire_వరుస_pos < Get_కొలత(wire_వరుస_entries)) {
			స్వయంచాలక wire = wire_వరుస_entries[wire_వరుస_pos++];
			xlog("processing wire %s\n", wire);

			ఒకవేళ (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString పోర్టు = wire->driver_పోర్టు_2;
				వరుసలో_చేర్చు_cell_పోర్టు(cell, పోర్టు);
			}

			అయితే (నిజం) {
				స్వయంచాలక దొరికింది = buf_norm_connect_సూచీ.కనుగొను(wire);
				ఒకవేళ (దొరికింది == buf_norm_connect_సూచీ.చివర())
					ఆపు;
				అయితే (!దొరికింది->రెండవ.ఖాళీలు()) {
					Cell *connect_cell = *దొరికింది->రెండవ.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec స్థిరరాశి &sig_a = connect_cell->get_పోర్టు(ID::A);
					SigSpec స్థిరరాశి &sig_b = connect_cell->get_పోర్టు(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					కొరకు (స్వయంచాలక &side : {sig_a, sig_b})
						కొరకు (స్వయంచాలక chunk : side.chunks())
							ఒకవేళ (chunk.wire)
								wire_వరుస_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_వరుస.చొప్పించు(connect_cell);
					remove(connect_cell);
					log_assert(Get_కొలత(buf_norm_wire_వరుస) <= 2);
					buf_norm_wire_వరుస.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		కొరకు (స్వయంచాలక &[cell, పోర్టు] : pending_ports) {
			log_assert(cell->type != ID($ఆదానం_పోర్టు));
			log_assert(!cell->type.ఖాళీలు());
			log_assert(!pending_deleted_cells.లెక్క(cell));
			SigSpec స్థిరరాశి &sig = cell->get_పోర్టు(పోర్టు);
			Wire *w = కూడు_Wire(NEW_ID, Get_కొలత(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			ప్రత్యక్ష_driven_wires.emplace(w, {cell, పోర్టు});
			cell->సమితి_పోర్టు(పోర్టు, w); // Hits the fast path that doesn't enqueue w
			wire_వరుస_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		కొరకు (స్వయంచాలక wire : wire_వరుస_entries) {
			wire->driverCell_ = శూన్యసూచిక;
			wire->driver_పోర్టు_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		కొరకు (స్వయంచాలక స్థిరరాశి &[wire, cellport] : ప్రత్యక్ష_driven_wires) {
			wire->driverCell_ = cellport.మొదటి;
			wire->driver_పోర్టు_2 = cellport.రెండవ;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		ఒకవేళ (yosys_xtrace)
			కొరకు (స్వయంచాలక స్థిరరాశి &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(ఇది);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		కొరకు (తార్కిక ఆదానం_ports : {అబద్ధం, నిజం}) {
			కొరకు (స్వయంచాలక స్థిరరాశి &[wire, cellport] : ప్రత్యక్ష_driven_wires) {
				ఒకవేళ ((wire->పోర్టు_ఆదానం && !wire->పోర్టు_output) == ఆదానం_ports) {
					కొరకు (పూర్ణాంకం i = 0; i != Get_కొలత(wire); ++i) {
						Sig_బిట్టు driver = Sig_బిట్టు(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_బిట్టు> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_బిట్టు> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_బిట్టు> weakly_driven;

		కొరకు (స్వయంచాలక స్థిరరాశి &[wire, cellport] : ప్రత్యక్ష_driven_wires) {
			స్వయంచాలక స్థిరరాశి &[cell, పోర్టు] = cellport;
			కొరకు (పూర్ణాంకం i = 0; i != Get_కొలత(wire); ++i) {
				Sig_బిట్టు driver = sigmap(Sig_బిట్టు(wire, i));
				ఒకవేళ (cell->type == ID($tribuf) || cell->పోర్టు_dir(పోర్టు) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.చొప్పించు(driver);
				} లేకపోతే {
					// We remove driver conflicts from `driven` in a separate loop below
					తార్కిక inserted = driven.చొప్పించు(driver).రెండవ;
					ఒకవేళ (!inserted)
						conflicted.చొప్పించు(driver);
				}
			}
		}

		కొరకు (స్వయంచాలక driver : weakly_driven)
			driven.చొప్పించు(driver);

		// This only leaves the drivers matching `driven`'s definition above
		కొరకు (స్వయంచాలక driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_బిట్టు, Sig_బిట్టు>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		కొరకు (స్వయంచాలక wire : wire_వరుస_entries) {
			// A zero-width wire carries no bits and needs no driver
			ఒకవేళ (Get_కొలత(wire) == 0)
				కొనసాగు;

			SigSpec wire_drivers;
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wire); ++i) {
				Sig_బిట్టు బిట్టు(wire, i);
				Sig_బిట్టు mapped = sigmap(బిట్టు);
				xlog("bit %s -> mapped %s\n", log_signal(బిట్టు), log_signal(mapped));


				buf_connected.apply(బిట్టు);
				buf_connected.కూడు(బిట్టు, mapped);
				buf_connected.database.promote(mapped);

				ఒకవేళ (wire->driverCell_ == శూన్యసూచిక) {
					ఒకవేళ (!mapped.is_wire() || driven.లెక్క(mapped)) {
						wire_drivers.append(mapped);
						కొనసాగు;
					} లేకపోతే {
						wire_drivers.append(State::Sz);
					}
				}

				ఒకవేళ (బిట్టు < mapped)
					undirected_connections.emplace(బిట్టు, mapped);
				లేకపోతే ఒకవేళ (mapped < బిట్టు)
					undirected_connections.emplace(mapped, బిట్టు);
			}

			ఒకవేళ (wire->driverCell_ == శూన్యసూచిక) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				కూడు_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		స్థిర స్వయంచాలక క్రమీకరించు_కీలకం = [](std::pair<Sig_బిట్టు, Sig_బిట్టు> స్థిరరాశి &p) {
			పూర్ణాంకం మొదటి_స్థానభ్రంశం = p.మొదటి.is_wire() ? p.మొదటి.స్థానభ్రంశం : 0;
			పూర్ణాంకం రెండవ_స్థానభ్రంశం = p.రెండవ.is_wire() ? p.రెండవ.స్థానభ్రంశం : 0;
			తిరిగి std::make_tuple(p.మొదటి.wire, p.రెండవ.wire, మొదటి_స్థానభ్రంశం - రెండవ_స్థానభ్రంశం, p);
		};

		undirected_connections.క్రమీకరించు([](std::pair<Sig_బిట్టు, Sig_బిట్టు> స్థిరరాశి &p, std::pair<Sig_బిట్టు, Sig_బిట్టు> స్థిరరాశి &q) {
			తిరిగి క్రమీకరించు_కీలకం(p) < క్రమీకరించు_కీలకం(q);
		});

		SigSpec tmp_a, tmp_b;

		కొరకు (స్వయంచాలక &[బిట్టు_a, బిట్టు_b] : undirected_connections) {
			tmp_a.append(బిట్టు_a);
			tmp_b.append(బిట్టు_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_బిట్టు తదుపరి_a, తదుపరి_b;

		స్వయంచాలక emit_connect_cell = [&]() {
			ఒకవేళ (sig_a.ఖాళీలు())
				తిరిగి;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = కూడు_Cell(NEW_ID, ID($connect));
			connect_cell->సమితి_Param(ID::వెడల్పు_2, Get_కొలత(sig_a));
			connect_cell->సమితి_పోర్టు(ID::A, sig_a);
			connect_cell->సమితి_పోర్టు(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		కొరకు (స్వయంచాలక &[బిట్టు_a, బిట్టు_b] : undirected_connections) {
			ఒకవేళ (బిట్టు_a == బిట్టు_b)
				కొనసాగు;
			ఒకవేళ (బిట్టు_a != తదుపరి_a || బిట్టు_b != తదుపరి_b)
				emit_connect_cell();

			sig_a.append(బిట్టు_a);
			sig_b.append(బిట్టు_b);
			తదుపరి_a = బిట్టు_a;
			తదుపరి_b = బిట్టు_b;
			ఒకవేళ (తదుపరి_a.is_wire())
				తదుపరి_a.స్థానభ్రంశం++;
			ఒకవేళ (తదుపరి_b.is_wire())
				తదుపరి_b.స్థానభ్రంశం++;

		}
		emit_connect_cell();

		buf_norm_cell_వరుస.clear();

		log_assert(buf_norm_cell_పోర్టు_వరుస.ఖాళీలు());
		log_assert(buf_norm_wire_వరుస.ఖాళీలు());
		log_assert(connections_.ఖాళీలు());
	}

	కొరకు (స్వయంచాలక cell : pending_deleted_cells) {
		తొలగించు cell;
	}
	pending_deleted_cells.clear();
}

శూన్య RTLIL::Cell::unset_పోర్టు(RTLIL::IdString portname)
{
	RTLIL::SigSpec సంకేతం;
	స్వయంచాలక conn_it = connections_.కనుగొను(portname);

	ఒకవేళ (conn_it != connections_.చివర())
	{
		కొరకు (స్వయంచాలక mon : module->monitors)
			mon->notify_connect(ఇది, conn_it->మొదటి, conn_it->రెండవ, సంకేతం);

		ఒకవేళ (module->design)
			కొరకు (స్వయంచాలక mon : module->design->monitors)
				mon->notify_connect(ఇది, conn_it->మొదటి, conn_it->రెండవ, సంకేతం);

		ఒకవేళ (yosys_xtrace) {
			సంవర్గమానం("#X# Unconnect %s.%s.%s\n", ఇది->module, ఇది, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		ఒకవేళ (module->design && module->design->flagBufferedNormalized) {
			ఒకవేళ (conn_it->రెండవ.is_wire()) {
				Wire *w = conn_it->రెండవ.as_wire();
				ఒకవేళ (w->driverCell_ == ఇది && w->driver_పోర్టు_2 == portname) {
					w->driverCell_ = శూన్యసూచిక;
					w->driver_పోర్టు_2 = IdString();
					module->buf_norm_wire_వరుస.చొప్పించు(w);
				} లేకపోతే ఒకవేళ (w->driverCell_) {
					log_assert(w->driverCell_->get_పోర్టు(w->driver_పోర్టు_2) == w);
				}
			}

			ఒకవేళ (type == ID($connect)) {
				కొరకు (స్వయంచాలక &[పోర్టు, sig] : connections_) {
					కొరకు (స్వయంచాలక &chunk : sig.chunks()) {
						ఒకవేళ (!chunk.wire)
							కొనసాగు;
						స్వయంచాలక it = module->buf_norm_connect_సూచీ.కనుగొను(chunk.wire);
						ఒకవేళ (it == module->buf_norm_connect_సూచీ.చివర())
							కొనసాగు;
						it->రెండవ.erase(ఇది);
						ఒకవేళ (it->రెండవ.ఖాళీలు())
							module->buf_norm_connect_సూచీ.erase(it);
					}
				}
				connections_.erase(conn_it);
				కొరకు (స్వయంచాలక &[పోర్టు, sig] : connections_) {
					కొరకు (స్వయంచాలక &chunk : sig.chunks()) {
						ఒకవేళ (!chunk.wire)
							కొనసాగు;
						module->buf_norm_connect_సూచీ[chunk.wire].చొప్పించు(ఇది);
					}
				}
				తిరిగి;
			}
		}

		connections_.erase(conn_it);
	}
}

శూన్య RTLIL::Cell::సమితి_పోర్టు(RTLIL::IdString portname, RTLIL::SigSpec సంకేతం)
{
	స్వయంచాలక r = connections_.చొప్పించు(portname);
	స్వయంచాలక conn_it = r.మొదటి;
	ఒకవేళ (!r.రెండవ && conn_it->రెండవ == సంకేతం)
		తిరిగి;

	కొరకు (స్వయంచాలక mon : module->monitors)
		mon->notify_connect(ఇది, conn_it->మొదటి, conn_it->రెండవ, సంకేతం);

	ఒకవేళ (module->design)
		కొరకు (స్వయంచాలక mon : module->design->monitors)
			mon->notify_connect(ఇది, conn_it->మొదటి, conn_it->రెండవ, సంకేతం);

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# Connect %s.%s.%s = %s (%d)\n", ఇది->module, ఇది, PooledName(module, portname).unescape(), log_signal(సంకేతం), Get_కొలత(సంకేతం));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ఒకవేళ (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		ఒకవేళ (conn_it->రెండవ.is_wire()) {
			Wire *w = conn_it->రెండవ.as_wire();
			ఒకవేళ (w->driverCell_ == ఇది && w->driver_పోర్టు_2 == portname) {
				w->driverCell_ = శూన్యసూచిక;
				w->driver_పోర్టు_2 = IdString();
				module->buf_norm_wire_వరుస.చొప్పించు(w);
			}
		}

		స్వయంచాలక dir = పోర్టు_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		ఒకవేళ ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && సంకేతం.is_wire()) {
			Wire *w = సంకేతం.as_wire();
			ఒకవేళ (w->driverCell_ == శూన్యసూచిక && (
						(w->పోర్టు_ఆదానం && !w->పోర్టు_output) == (type == ID($ఆదానం_పోర్టు)))) {
				w->driverCell_ = ఇది;
				w->driver_పోర్టు_2 = portname;

				conn_it->రెండవ = std::move(సంకేతం);
				తిరిగి;
			}
		}

		ఒకవేళ (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_వరుస.చొప్పించు(ఇది);
			module->buf_norm_cell_పోర్టు_వరుస.emplace(ఇది, portname);
		} లేకపోతే {
			కొరకు (స్వయంచాలక &chunk : సంకేతం.chunks())
				ఒకవేళ (chunk.wire != శూన్యసూచిక && chunk.wire->driverCell_ == శూన్యసూచిక)
					module->buf_norm_wire_వరుస.చొప్పించు(chunk.wire);
		}

		ఒకవేళ (type == ID($connect)) {
			కొరకు (స్వయంచాలక &[పోర్టు, sig] : connections_) {
				కొరకు (స్వయంచాలక &chunk : sig.chunks()) {
					ఒకవేళ (!chunk.wire)
						కొనసాగు;
					స్వయంచాలక it = module->buf_norm_connect_సూచీ.కనుగొను(chunk.wire);
					ఒకవేళ (it == module->buf_norm_connect_సూచీ.చివర())
						కొనసాగు;
					it->రెండవ.erase(ఇది);
					ఒకవేళ (it->రెండవ.ఖాళీలు())
						module->buf_norm_connect_సూచీ.erase(it);
				}
			}
			conn_it->రెండవ = std::move(సంకేతం);
			కొరకు (స్వయంచాలక &[పోర్టు, sig] : connections_) {
				కొరకు (స్వయంచాలక &chunk : sig.chunks()) {
					ఒకవేళ (!chunk.wire)
						కొనసాగు;
					module->buf_norm_connect_సూచీ[chunk.wire].చొప్పించు(ఇది);
				}
			}
			తిరిగి;
		}
	}
	conn_it->రెండవ = std::move(సంకేతం);

}

శూన్య RTLIL::Design::కూడు(RTLIL::Module *module)
{
	log_assert(modules_.లెక్క(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = ఇది;

	కొరకు (స్వయంచాలక mon : monitors)
		mon->notify_module_కూడు(module);

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ఒకవేళ (flagBufferedNormalized) {
		buf_norm_విత్తనం_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_చివర
