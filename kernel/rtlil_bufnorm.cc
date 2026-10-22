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

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/modtools.h"

#ضمن <string.h>
#ضمن <algorithm>
#ضمن <optional>

YOSYS_NAMESPACE_BEGIN


ساكن فراغ buf_norm_ٻج_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	لكل (خودڪار cell : module->cells())
	لكل (خودڪار &conn : cell->connections()) {
		إذا (Get_سائيز(conn.ٻيو) == 0 || (cell->پورٽ_dir(conn.پهريون) != RTLIL::PD_OUTPUT && cell->پورٽ_dir(conn.پهريون) != RTLIL::PD_INOUT))
			استمر;
		module->buf_norm_cell_قطار.وجهو(cell);
		module->buf_norm_cell_پورٽ_قطار.emplace(cell, conn.پهريون);
	}
	لكل (خودڪار wire : module->wires())
		module->buf_norm_wire_قطار.وجهو(wire);
}

فراغ RTLIL::Design::bufNormalize(منطقي enable)
{
	إذا (!enable)
	{
		إذا (!flagBufferedNormalized)
			أرجع;

		لكل (خودڪار module : modules()) {
			module->buf_norm_cell_قطار.clear();
			module->buf_norm_wire_قطار.clear();
			module->buf_norm_cell_پورٽ_قطار.clear();
			لكل (خودڪار wire : module->wires()) {
				wire->driverCell_ = خالي_اشارو;
				wire->driver_پورٽ_2 = IdString();
			}
			module->buf_norm_connect_اشاريو.clear();
		}

		flagBufferedNormalized = خطأ;
		أرجع;
	}

	إذا (!flagBufferedNormalized)
	{
		لكل (خودڪار module : modules())
			buf_norm_ٻج_queues(module);

		flagBufferedNormalized = سچ;
	}

	لكل (خودڪار module : modules())
		module->bufNormalize();
}

بنية ٻٽو_انگ_drive_معلومات_t {
	صحيح drivers = 0;
	صحيح inout = 0;
	صحيح users = 0;
};

تعريفنوع ModWalker::پورٽ_ٻٽو_انگ_2 پورٽ_ٻٽو_انگ_2;

فراغ RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#وصف_ڪريو xlog(...) افعل { إذا (yosys_xtrace) لاگرٿم("#X [bufnorm] " __VA_ARGS__); } طالما (0)

	إذا (!design->flagBufferedNormalized)
		أرجع;

	إذا (!buf_norm_cell_قطار.خالي_جايون() || !buf_norm_wire_قطار.خالي_جايون() || !connections_.خالي_جايون())
	{
		// Ensure that every enqueued input port is represented by a cell
		لكل (خودڪار wire : buf_norm_wire_قطار) {
			إذا (wire->پورٽ_داخلا && !wire->پورٽ_output && Get_سائيز(wire) != 0) {
				إذا (wire->driverCell_ != خالي_اشارو && wire->driverCell_->type != ID($داخلا_پورٽ)) {
					wire->driverCell_ = خالي_اشارو;
					wire->driver_پورٽ_2 = IdString::Null;
				}
				إذا (wire->driverCell_ == خالي_اشارو) {
					Cell *داخلا_پورٽ_cell = جوڙيو_Cell(NEW_ID, ID($داخلا_پورٽ));
					داخلا_پورٽ_cell->سيٽ_Param(ID::ويڪر_2, Get_سائيز(wire));
					داخلا_پورٽ_cell->سيٽ_پورٽ(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
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

		idict<Wire *> wire_قطار_entries; // Ordered queue of wires to process
		صحيح wire_قطار_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> سڌو_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		خودڪار قطار_اندر_وجهو_cell_پورٽ = [&](Cell *cell, IdString پورٽ) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(پورٽ));

			// An empty cell type means the cell got removed
			إذا (cell->type.خالي_جايون())
				أرجع;


			SigSpec ثابت &sig = cell->get_پورٽ(پورٽ);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			لكل (خودڪار chunk : sig.chunks())
				إذا (chunk.is_wire())
					wire_قطار_entries(chunk.wire);

			إذا (cell->type == ID($buf) && cell->خاصيتون.خالي_جايون() && !cell->name.is_عوامي()) {
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
				log_assert(پورٽ == ID::Y);
				SigSpec sig_a = cell->get_پورٽ(ID::A);
				SigSpec sig_y = sig;

				لكل (خودڪار ثابت &s : {sig_a, sig})
					لكل (خودڪار ثابت &chunk : s.chunks())
						إذا (chunk.wire)
							wire_قطار_entries(chunk.wire);

				إذا (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					لكل (صحيح i = 0; i < Get_سائيز(sig_a); ++i) {
						Sig_ٻٽو_انگ b = sig_a[i];
						إذا (b == State::Sz)
							استمر;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				إذا (!sig_y.خالي_جايون())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_سائيز(buf_norm_wire_قطار) <= 1);
				buf_norm_wire_قطار.clear();
				أرجع;
			} وإلا إذا (cell->type == ID($داخلا_پورٽ)) {
				log_assert(پورٽ == ID::Y);
				إذا (sig.is_wire()) {
					Wire *w = sig.as_wire();
					إذا (w->پورٽ_داخلا && !w->پورٽ_output) {
						// An $input_port cell can only drive a full wire module input port
						خودڪار [مليو, inserted] = سڌو_driven_wires.emplace(w, {cell, پورٽ});
						إذا (!inserted || (مليو->ٻيو.پهريون == cell && مليو->ٻيو.ٻيو == پورٽ))
							أرجع;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_قطار.وجهو(cell);
				remove(cell);
				log_assert(Get_سائيز(buf_norm_wire_قطار) <= 1);
				buf_norm_wire_قطار.clear();
				أرجع;
			}

			إذا (sig.is_wire()) {
				Wire *w = sig.as_wire();
				إذا (!w->پورٽ_داخلا || w->پورٽ_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					خودڪار [مليو, inserted] = سڌو_driven_wires.emplace(w, {cell, پورٽ});
					إذا (inserted || (مليو->ٻيو.پهريون == cell && مليو->ٻيو.ٻيو == پورٽ))
						أرجع;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, پورٽ);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		لكل (خودڪار wire : buf_norm_wire_قطار)
			wire_قطار_entries(wire);
		buf_norm_wire_قطار.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		لكل (خودڪار ثابت &[cell, پورٽ_name] : buf_norm_cell_پورٽ_قطار)
			قطار_اندر_وجهو_cell_پورٽ(cell, پورٽ_name);
		buf_norm_cell_پورٽ_قطار.clear();

		// We also enqueue all wires that saw newly added module level connections.
		لكل (خودڪار &[a, b] : connections_)
			لكل (خودڪار &sig : {a, b})
				لكل (خودڪار ثابت &chunk : sig.chunks())
					إذا (chunk.wire)
						wire_قطار_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		طالما (wire_قطار_pos < Get_سائيز(wire_قطار_entries)) {
			خودڪار wire = wire_قطار_entries[wire_قطار_pos++];
			xlog("processing wire %s\n", wire);

			إذا (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString پورٽ = wire->driver_پورٽ_2;
				قطار_اندر_وجهو_cell_پورٽ(cell, پورٽ);
			}

			طالما (سچ) {
				خودڪار مليو = buf_norm_connect_اشاريو.ڳوليو(wire);
				إذا (مليو == buf_norm_connect_اشاريو.پڇاڙي())
					اكسر;
				طالما (!مليو->ٻيو.خالي_جايون()) {
					Cell *connect_cell = *مليو->ٻيو.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ثابت &sig_a = connect_cell->get_پورٽ(ID::A);
					SigSpec ثابت &sig_b = connect_cell->get_پورٽ(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					لكل (خودڪار &side : {sig_a, sig_b})
						لكل (خودڪار chunk : side.chunks())
							إذا (chunk.wire)
								wire_قطار_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_قطار.وجهو(connect_cell);
					remove(connect_cell);
					log_assert(Get_سائيز(buf_norm_wire_قطار) <= 2);
					buf_norm_wire_قطار.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		لكل (خودڪار &[cell, پورٽ] : pending_ports) {
			log_assert(cell->type != ID($داخلا_پورٽ));
			log_assert(!cell->type.خالي_جايون());
			log_assert(!pending_deleted_cells.ڳڻپ(cell));
			SigSpec ثابت &sig = cell->get_پورٽ(پورٽ);
			Wire *w = جوڙيو_Wire(NEW_ID, Get_سائيز(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			سڌو_driven_wires.emplace(w, {cell, پورٽ});
			cell->سيٽ_پورٽ(پورٽ, w); // Hits the fast path that doesn't enqueue w
			wire_قطار_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		لكل (خودڪار wire : wire_قطار_entries) {
			wire->driverCell_ = خالي_اشارو;
			wire->driver_پورٽ_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		لكل (خودڪار ثابت &[wire, cellport] : سڌو_driven_wires) {
			wire->driverCell_ = cellport.پهريون;
			wire->driver_پورٽ_2 = cellport.ٻيو;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		إذا (yosys_xtrace)
			لكل (خودڪار ثابت &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(هذا);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		لكل (منطقي داخلا_ports : {خطأ, سچ}) {
			لكل (خودڪار ثابت &[wire, cellport] : سڌو_driven_wires) {
				إذا ((wire->پورٽ_داخلا && !wire->پورٽ_output) == داخلا_ports) {
					لكل (صحيح i = 0; i != Get_سائيز(wire); ++i) {
						Sig_ٻٽو_انگ driver = Sig_ٻٽو_انگ(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_ٻٽو_انگ> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_ٻٽو_انگ> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_ٻٽو_انگ> weakly_driven;

		لكل (خودڪار ثابت &[wire, cellport] : سڌو_driven_wires) {
			خودڪار ثابت &[cell, پورٽ] = cellport;
			لكل (صحيح i = 0; i != Get_سائيز(wire); ++i) {
				Sig_ٻٽو_انگ driver = sigmap(Sig_ٻٽو_انگ(wire, i));
				إذا (cell->type == ID($tribuf) || cell->پورٽ_dir(پورٽ) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.وجهو(driver);
				} وإلا {
					// We remove driver conflicts from `driven` in a separate loop below
					منطقي inserted = driven.وجهو(driver).ٻيو;
					إذا (!inserted)
						conflicted.وجهو(driver);
				}
			}
		}

		لكل (خودڪار driver : weakly_driven)
			driven.وجهو(driver);

		// This only leaves the drivers matching `driven`'s definition above
		لكل (خودڪار driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		لكل (خودڪار wire : wire_قطار_entries) {
			// A zero-width wire carries no bits and needs no driver
			إذا (Get_سائيز(wire) == 0)
				استمر;

			SigSpec wire_drivers;
			لكل (صحيح i = 0; i < Get_سائيز(wire); ++i) {
				Sig_ٻٽو_انگ ٻٽو_انگ(wire, i);
				Sig_ٻٽو_انگ mapped = sigmap(ٻٽو_انگ);
				xlog("bit %s -> mapped %s\n", log_signal(ٻٽو_انگ), log_signal(mapped));


				buf_connected.apply(ٻٽو_انگ);
				buf_connected.جوڙيو(ٻٽو_انگ, mapped);
				buf_connected.database.promote(mapped);

				إذا (wire->driverCell_ == خالي_اشارو) {
					إذا (!mapped.is_wire() || driven.ڳڻپ(mapped)) {
						wire_drivers.append(mapped);
						استمر;
					} وإلا {
						wire_drivers.append(State::Sz);
					}
				}

				إذا (ٻٽو_انگ < mapped)
					undirected_connections.emplace(ٻٽو_انگ, mapped);
				وإلا إذا (mapped < ٻٽو_انگ)
					undirected_connections.emplace(mapped, ٻٽو_انگ);
			}

			إذا (wire->driverCell_ == خالي_اشارو) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				جوڙيو_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ساكن خودڪار ترتيب_ڏيو_ڪنجي = [](std::pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ> ثابت &p) {
			صحيح پهريون_وٿي = p.پهريون.is_wire() ? p.پهريون.وٿي : 0;
			صحيح ٻيو_وٿي = p.ٻيو.is_wire() ? p.ٻيو.وٿي : 0;
			أرجع std::make_tuple(p.پهريون.wire, p.ٻيو.wire, پهريون_وٿي - ٻيو_وٿي, p);
		};

		undirected_connections.ترتيب_ڏيو([](std::pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ> ثابت &p, std::pair<Sig_ٻٽو_انگ, Sig_ٻٽو_انگ> ثابت &q) {
			أرجع ترتيب_ڏيو_ڪنجي(p) < ترتيب_ڏيو_ڪنجي(q);
		});

		SigSpec tmp_a, tmp_b;

		لكل (خودڪار &[ٻٽو_انگ_a, ٻٽو_انگ_b] : undirected_connections) {
			tmp_a.append(ٻٽو_انگ_a);
			tmp_b.append(ٻٽو_انگ_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_ٻٽو_انگ ايندڙ_a, ايندڙ_b;

		خودڪار emit_connect_cell = [&]() {
			إذا (sig_a.خالي_جايون())
				أرجع;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = جوڙيو_Cell(NEW_ID, ID($connect));
			connect_cell->سيٽ_Param(ID::ويڪر_2, Get_سائيز(sig_a));
			connect_cell->سيٽ_پورٽ(ID::A, sig_a);
			connect_cell->سيٽ_پورٽ(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		لكل (خودڪار &[ٻٽو_انگ_a, ٻٽو_انگ_b] : undirected_connections) {
			إذا (ٻٽو_انگ_a == ٻٽو_انگ_b)
				استمر;
			إذا (ٻٽو_انگ_a != ايندڙ_a || ٻٽو_انگ_b != ايندڙ_b)
				emit_connect_cell();

			sig_a.append(ٻٽو_انگ_a);
			sig_b.append(ٻٽو_انگ_b);
			ايندڙ_a = ٻٽو_انگ_a;
			ايندڙ_b = ٻٽو_انگ_b;
			إذا (ايندڙ_a.is_wire())
				ايندڙ_a.وٿي++;
			إذا (ايندڙ_b.is_wire())
				ايندڙ_b.وٿي++;

		}
		emit_connect_cell();

		buf_norm_cell_قطار.clear();

		log_assert(buf_norm_cell_پورٽ_قطار.خالي_جايون());
		log_assert(buf_norm_wire_قطار.خالي_جايون());
		log_assert(connections_.خالي_جايون());
	}

	لكل (خودڪار cell : pending_deleted_cells) {
		ميٽيو cell;
	}
	pending_deleted_cells.clear();
}

فراغ RTLIL::Cell::unset_پورٽ(RTLIL::IdString portname)
{
	RTLIL::SigSpec إشارة;
	خودڪار conn_it = connections_.ڳوليو(portname);

	إذا (conn_it != connections_.پڇاڙي())
	{
		لكل (خودڪار mon : module->monitors)
			mon->notify_connect(هذا, conn_it->پهريون, conn_it->ٻيو, إشارة);

		إذا (module->design)
			لكل (خودڪار mon : module->design->monitors)
				mon->notify_connect(هذا, conn_it->پهريون, conn_it->ٻيو, إشارة);

		إذا (yosys_xtrace) {
			لاگرٿم("#X# Unconnect %s.%s.%s\n", هذا->module, هذا, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		إذا (module->design && module->design->flagBufferedNormalized) {
			إذا (conn_it->ٻيو.is_wire()) {
				Wire *w = conn_it->ٻيو.as_wire();
				إذا (w->driverCell_ == هذا && w->driver_پورٽ_2 == portname) {
					w->driverCell_ = خالي_اشارو;
					w->driver_پورٽ_2 = IdString();
					module->buf_norm_wire_قطار.وجهو(w);
				} وإلا إذا (w->driverCell_) {
					log_assert(w->driverCell_->get_پورٽ(w->driver_پورٽ_2) == w);
				}
			}

			إذا (type == ID($connect)) {
				لكل (خودڪار &[پورٽ, sig] : connections_) {
					لكل (خودڪار &chunk : sig.chunks()) {
						إذا (!chunk.wire)
							استمر;
						خودڪار it = module->buf_norm_connect_اشاريو.ڳوليو(chunk.wire);
						إذا (it == module->buf_norm_connect_اشاريو.پڇاڙي())
							استمر;
						it->ٻيو.erase(هذا);
						إذا (it->ٻيو.خالي_جايون())
							module->buf_norm_connect_اشاريو.erase(it);
					}
				}
				connections_.erase(conn_it);
				لكل (خودڪار &[پورٽ, sig] : connections_) {
					لكل (خودڪار &chunk : sig.chunks()) {
						إذا (!chunk.wire)
							استمر;
						module->buf_norm_connect_اشاريو[chunk.wire].وجهو(هذا);
					}
				}
				أرجع;
			}
		}

		connections_.erase(conn_it);
	}
}

فراغ RTLIL::Cell::سيٽ_پورٽ(RTLIL::IdString portname, RTLIL::SigSpec إشارة)
{
	خودڪار r = connections_.وجهو(portname);
	خودڪار conn_it = r.پهريون;
	إذا (!r.ٻيو && conn_it->ٻيو == إشارة)
		أرجع;

	لكل (خودڪار mon : module->monitors)
		mon->notify_connect(هذا, conn_it->پهريون, conn_it->ٻيو, إشارة);

	إذا (module->design)
		لكل (خودڪار mon : module->design->monitors)
			mon->notify_connect(هذا, conn_it->پهريون, conn_it->ٻيو, إشارة);

	إذا (yosys_xtrace) {
		لاگرٿم("#X# Connect %s.%s.%s = %s (%d)\n", هذا->module, هذا, PooledName(module, portname).unescape(), log_signal(إشارة), Get_سائيز(إشارة));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	إذا (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		إذا (conn_it->ٻيو.is_wire()) {
			Wire *w = conn_it->ٻيو.as_wire();
			إذا (w->driverCell_ == هذا && w->driver_پورٽ_2 == portname) {
				w->driverCell_ = خالي_اشارو;
				w->driver_پورٽ_2 = IdString();
				module->buf_norm_wire_قطار.وجهو(w);
			}
		}

		خودڪار dir = پورٽ_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		إذا ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && إشارة.is_wire()) {
			Wire *w = إشارة.as_wire();
			إذا (w->driverCell_ == خالي_اشارو && (
						(w->پورٽ_داخلا && !w->پورٽ_output) == (type == ID($داخلا_پورٽ)))) {
				w->driverCell_ = هذا;
				w->driver_پورٽ_2 = portname;

				conn_it->ٻيو = std::move(إشارة);
				أرجع;
			}
		}

		إذا (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_قطار.وجهو(هذا);
			module->buf_norm_cell_پورٽ_قطار.emplace(هذا, portname);
		} وإلا {
			لكل (خودڪار &chunk : إشارة.chunks())
				إذا (chunk.wire != خالي_اشارو && chunk.wire->driverCell_ == خالي_اشارو)
					module->buf_norm_wire_قطار.وجهو(chunk.wire);
		}

		إذا (type == ID($connect)) {
			لكل (خودڪار &[پورٽ, sig] : connections_) {
				لكل (خودڪار &chunk : sig.chunks()) {
					إذا (!chunk.wire)
						استمر;
					خودڪار it = module->buf_norm_connect_اشاريو.ڳوليو(chunk.wire);
					إذا (it == module->buf_norm_connect_اشاريو.پڇاڙي())
						استمر;
					it->ٻيو.erase(هذا);
					إذا (it->ٻيو.خالي_جايون())
						module->buf_norm_connect_اشاريو.erase(it);
				}
			}
			conn_it->ٻيو = std::move(إشارة);
			لكل (خودڪار &[پورٽ, sig] : connections_) {
				لكل (خودڪار &chunk : sig.chunks()) {
					إذا (!chunk.wire)
						استمر;
					module->buf_norm_connect_اشاريو[chunk.wire].وجهو(هذا);
				}
			}
			أرجع;
		}
	}
	conn_it->ٻيو = std::move(إشارة);

}

فراغ RTLIL::Design::جوڙيو(RTLIL::Module *module)
{
	log_assert(modules_.ڳڻپ(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = هذا;

	لكل (خودڪار mon : monitors)
		mon->notify_module_جوڙيو(module);

	إذا (yosys_xtrace) {
		لاگرٿم("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	إذا (flagBufferedNormalized) {
		buf_norm_ٻج_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_پڇاڙي
