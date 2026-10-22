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

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/modtools.h"

#شامل <string.h>
#شامل <algorithm>
#شامل <optional>

YOSYS_NAMESPACE_BEGIN


ساکن خالی buf_norm_بیج_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	برائے (خودکار cell : module->cells())
	برائے (خودکار &conn : cell->connections()) {
		اگر (Get_سائز(conn.دوسرا) == 0 || (cell->پورٹ_dir(conn.پہلا) != RTLIL::PD_OUTPUT && cell->پورٹ_dir(conn.پہلا) != RTLIL::PD_INOUT))
			جاری;
		module->buf_norm_cell_قطار.داخل_کرو(cell);
		module->buf_norm_cell_پورٹ_قطار.emplace(cell, conn.پہلا);
	}
	برائے (خودکار wire : module->wires())
		module->buf_norm_wire_قطار.داخل_کرو(wire);
}

خالی RTLIL::Design::bufNormalize(منطقی enable)
{
	اگر (!enable)
	{
		اگر (!flagBufferedNormalized)
			واپس;

		برائے (خودکار module : modules()) {
			module->buf_norm_cell_قطار.clear();
			module->buf_norm_wire_قطار.clear();
			module->buf_norm_cell_پورٹ_قطار.clear();
			برائے (خودکار wire : module->wires()) {
				wire->driverCell_ = خالی_اشاریہ;
				wire->driver_پورٹ_2 = IdString();
			}
			module->buf_norm_connect_اشاریہ.clear();
		}

		flagBufferedNormalized = جھوٹ;
		واپس;
	}

	اگر (!flagBufferedNormalized)
	{
		برائے (خودکار module : modules())
			buf_norm_بیج_queues(module);

		flagBufferedNormalized = سچ;
	}

	برائے (خودکار module : modules())
		module->bufNormalize();
}

ساخت ثنائی_ہندسہ_drive_ڈیٹا_t {
	صحیح drivers = 0;
	صحیح inout = 0;
	صحیح users = 0;
};

قسمتعریف ModWalker::پورٹ_ثنائی_ہندسہ_2 پورٹ_ثنائی_ہندسہ_2;

خالی RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#تعریف_کرو xlog(...) کرو { اگر (yosys_xtrace) لوگارتھم("#X [bufnorm] " __VA_ARGS__); } جب (0)

	اگر (!design->flagBufferedNormalized)
		واپس;

	اگر (!buf_norm_cell_قطار.خالی_خانے() || !buf_norm_wire_قطار.خالی_خانے() || !connections_.خالی_خانے())
	{
		// Ensure that every enqueued input port is represented by a cell
		برائے (خودکار wire : buf_norm_wire_قطار) {
			اگر (wire->پورٹ_داخلہ && !wire->پورٹ_output && Get_سائز(wire) != 0) {
				اگر (wire->driverCell_ != خالی_اشاریہ && wire->driverCell_->type != ID($داخلہ_پورٹ)) {
					wire->driverCell_ = خالی_اشاریہ;
					wire->driver_پورٹ_2 = IdString::Null;
				}
				اگر (wire->driverCell_ == خالی_اشاریہ) {
					Cell *داخلہ_پورٹ_cell = جوڑو_Cell(NEW_ID, ID($داخلہ_پورٹ));
					داخلہ_پورٹ_cell->سیٹ_Param(ID::چوڑائی_2, Get_سائز(wire));
					داخلہ_پورٹ_cell->سیٹ_پورٹ(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
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
		صحیح wire_قطار_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> براہ_راست_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		خودکار قطار_میں_ڈالو_cell_پورٹ = [&](Cell *cell, IdString پورٹ) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(پورٹ));

			// An empty cell type means the cell got removed
			اگر (cell->type.خالی_خانے())
				واپس;


			SigSpec ثابت &sig = cell->get_پورٹ(پورٹ);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			برائے (خودکار chunk : sig.chunks())
				اگر (chunk.is_wire())
					wire_قطار_entries(chunk.wire);

			اگر (cell->type == ID($buf) && cell->اوصاف.خالی_خانے() && !cell->name.is_عوامی()) {
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
				log_assert(پورٹ == ID::Y);
				SigSpec sig_a = cell->get_پورٹ(ID::A);
				SigSpec sig_y = sig;

				برائے (خودکار ثابت &s : {sig_a, sig})
					برائے (خودکار ثابت &chunk : s.chunks())
						اگر (chunk.wire)
							wire_قطار_entries(chunk.wire);

				اگر (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					برائے (صحیح i = 0; i < Get_سائز(sig_a); ++i) {
						Sig_ثنائی_ہندسہ b = sig_a[i];
						اگر (b == State::Sz)
							جاری;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				اگر (!sig_y.خالی_خانے())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_سائز(buf_norm_wire_قطار) <= 1);
				buf_norm_wire_قطار.clear();
				واپس;
			} ورنہ اگر (cell->type == ID($داخلہ_پورٹ)) {
				log_assert(پورٹ == ID::Y);
				اگر (sig.is_wire()) {
					Wire *w = sig.as_wire();
					اگر (w->پورٹ_داخلہ && !w->پورٹ_output) {
						// An $input_port cell can only drive a full wire module input port
						خودکار [ملا, inserted] = براہ_راست_driven_wires.emplace(w, {cell, پورٹ});
						اگر (!inserted || (ملا->دوسرا.پہلا == cell && ملا->دوسرا.دوسرا == پورٹ))
							واپس;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_قطار.داخل_کرو(cell);
				remove(cell);
				log_assert(Get_سائز(buf_norm_wire_قطار) <= 1);
				buf_norm_wire_قطار.clear();
				واپس;
			}

			اگر (sig.is_wire()) {
				Wire *w = sig.as_wire();
				اگر (!w->پورٹ_داخلہ || w->پورٹ_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					خودکار [ملا, inserted] = براہ_راست_driven_wires.emplace(w, {cell, پورٹ});
					اگر (inserted || (ملا->دوسرا.پہلا == cell && ملا->دوسرا.دوسرا == پورٹ))
						واپس;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, پورٹ);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		برائے (خودکار wire : buf_norm_wire_قطار)
			wire_قطار_entries(wire);
		buf_norm_wire_قطار.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		برائے (خودکار ثابت &[cell, پورٹ_name] : buf_norm_cell_پورٹ_قطار)
			قطار_میں_ڈالو_cell_پورٹ(cell, پورٹ_name);
		buf_norm_cell_پورٹ_قطار.clear();

		// We also enqueue all wires that saw newly added module level connections.
		برائے (خودکار &[a, b] : connections_)
			برائے (خودکار &sig : {a, b})
				برائے (خودکار ثابت &chunk : sig.chunks())
					اگر (chunk.wire)
						wire_قطار_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		جب (wire_قطار_pos < Get_سائز(wire_قطار_entries)) {
			خودکار wire = wire_قطار_entries[wire_قطار_pos++];
			xlog("processing wire %s\n", wire);

			اگر (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString پورٹ = wire->driver_پورٹ_2;
				قطار_میں_ڈالو_cell_پورٹ(cell, پورٹ);
			}

			جب (سچ) {
				خودکار ملا = buf_norm_connect_اشاریہ.ڈھونڈو(wire);
				اگر (ملا == buf_norm_connect_اشاریہ.انتہا())
					توڑو;
				جب (!ملا->دوسرا.خالی_خانے()) {
					Cell *connect_cell = *ملا->دوسرا.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ثابت &sig_a = connect_cell->get_پورٹ(ID::A);
					SigSpec ثابت &sig_b = connect_cell->get_پورٹ(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					برائے (خودکار &side : {sig_a, sig_b})
						برائے (خودکار chunk : side.chunks())
							اگر (chunk.wire)
								wire_قطار_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_قطار.داخل_کرو(connect_cell);
					remove(connect_cell);
					log_assert(Get_سائز(buf_norm_wire_قطار) <= 2);
					buf_norm_wire_قطار.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		برائے (خودکار &[cell, پورٹ] : pending_ports) {
			log_assert(cell->type != ID($داخلہ_پورٹ));
			log_assert(!cell->type.خالی_خانے());
			log_assert(!pending_deleted_cells.گنتی(cell));
			SigSpec ثابت &sig = cell->get_پورٹ(پورٹ);
			Wire *w = جوڑو_Wire(NEW_ID, Get_سائز(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			براہ_راست_driven_wires.emplace(w, {cell, پورٹ});
			cell->سیٹ_پورٹ(پورٹ, w); // Hits the fast path that doesn't enqueue w
			wire_قطار_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		برائے (خودکار wire : wire_قطار_entries) {
			wire->driverCell_ = خالی_اشاریہ;
			wire->driver_پورٹ_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		برائے (خودکار ثابت &[wire, cellport] : براہ_راست_driven_wires) {
			wire->driverCell_ = cellport.پہلا;
			wire->driver_پورٹ_2 = cellport.دوسرا;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		اگر (yosys_xtrace)
			برائے (خودکار ثابت &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(یہ);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		برائے (منطقی داخلہ_ports : {جھوٹ, سچ}) {
			برائے (خودکار ثابت &[wire, cellport] : براہ_راست_driven_wires) {
				اگر ((wire->پورٹ_داخلہ && !wire->پورٹ_output) == داخلہ_ports) {
					برائے (صحیح i = 0; i != Get_سائز(wire); ++i) {
						Sig_ثنائی_ہندسہ driver = Sig_ثنائی_ہندسہ(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_ثنائی_ہندسہ> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_ثنائی_ہندسہ> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_ثنائی_ہندسہ> weakly_driven;

		برائے (خودکار ثابت &[wire, cellport] : براہ_راست_driven_wires) {
			خودکار ثابت &[cell, پورٹ] = cellport;
			برائے (صحیح i = 0; i != Get_سائز(wire); ++i) {
				Sig_ثنائی_ہندسہ driver = sigmap(Sig_ثنائی_ہندسہ(wire, i));
				اگر (cell->type == ID($tribuf) || cell->پورٹ_dir(پورٹ) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.داخل_کرو(driver);
				} ورنہ {
					// We remove driver conflicts from `driven` in a separate loop below
					منطقی inserted = driven.داخل_کرو(driver).دوسرا;
					اگر (!inserted)
						conflicted.داخل_کرو(driver);
				}
			}
		}

		برائے (خودکار driver : weakly_driven)
			driven.داخل_کرو(driver);

		// This only leaves the drivers matching `driven`'s definition above
		برائے (خودکار driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		برائے (خودکار wire : wire_قطار_entries) {
			// A zero-width wire carries no bits and needs no driver
			اگر (Get_سائز(wire) == 0)
				جاری;

			SigSpec wire_drivers;
			برائے (صحیح i = 0; i < Get_سائز(wire); ++i) {
				Sig_ثنائی_ہندسہ ثنائی_ہندسہ(wire, i);
				Sig_ثنائی_ہندسہ mapped = sigmap(ثنائی_ہندسہ);
				xlog("bit %s -> mapped %s\n", log_signal(ثنائی_ہندسہ), log_signal(mapped));


				buf_connected.apply(ثنائی_ہندسہ);
				buf_connected.جوڑو(ثنائی_ہندسہ, mapped);
				buf_connected.database.promote(mapped);

				اگر (wire->driverCell_ == خالی_اشاریہ) {
					اگر (!mapped.is_wire() || driven.گنتی(mapped)) {
						wire_drivers.append(mapped);
						جاری;
					} ورنہ {
						wire_drivers.append(State::Sz);
					}
				}

				اگر (ثنائی_ہندسہ < mapped)
					undirected_connections.emplace(ثنائی_ہندسہ, mapped);
				ورنہ اگر (mapped < ثنائی_ہندسہ)
					undirected_connections.emplace(mapped, ثنائی_ہندسہ);
			}

			اگر (wire->driverCell_ == خالی_اشاریہ) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				جوڑو_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ساکن خودکار ترتیب_دو_کلید = [](std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ> ثابت &p) {
			صحیح پہلا_انحراف = p.پہلا.is_wire() ? p.پہلا.انحراف : 0;
			صحیح دوسرا_انحراف = p.دوسرا.is_wire() ? p.دوسرا.انحراف : 0;
			واپس std::make_tuple(p.پہلا.wire, p.دوسرا.wire, پہلا_انحراف - دوسرا_انحراف, p);
		};

		undirected_connections.ترتیب_دو([](std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ> ثابت &p, std::pair<Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ> ثابت &q) {
			واپس ترتیب_دو_کلید(p) < ترتیب_دو_کلید(q);
		});

		SigSpec tmp_a, tmp_b;

		برائے (خودکار &[ثنائی_ہندسہ_a, ثنائی_ہندسہ_b] : undirected_connections) {
			tmp_a.append(ثنائی_ہندسہ_a);
			tmp_b.append(ثنائی_ہندسہ_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_ثنائی_ہندسہ اگلا_a, اگلا_b;

		خودکار emit_connect_cell = [&]() {
			اگر (sig_a.خالی_خانے())
				واپس;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = جوڑو_Cell(NEW_ID, ID($connect));
			connect_cell->سیٹ_Param(ID::چوڑائی_2, Get_سائز(sig_a));
			connect_cell->سیٹ_پورٹ(ID::A, sig_a);
			connect_cell->سیٹ_پورٹ(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		برائے (خودکار &[ثنائی_ہندسہ_a, ثنائی_ہندسہ_b] : undirected_connections) {
			اگر (ثنائی_ہندسہ_a == ثنائی_ہندسہ_b)
				جاری;
			اگر (ثنائی_ہندسہ_a != اگلا_a || ثنائی_ہندسہ_b != اگلا_b)
				emit_connect_cell();

			sig_a.append(ثنائی_ہندسہ_a);
			sig_b.append(ثنائی_ہندسہ_b);
			اگلا_a = ثنائی_ہندسہ_a;
			اگلا_b = ثنائی_ہندسہ_b;
			اگر (اگلا_a.is_wire())
				اگلا_a.انحراف++;
			اگر (اگلا_b.is_wire())
				اگلا_b.انحراف++;

		}
		emit_connect_cell();

		buf_norm_cell_قطار.clear();

		log_assert(buf_norm_cell_پورٹ_قطار.خالی_خانے());
		log_assert(buf_norm_wire_قطار.خالی_خانے());
		log_assert(connections_.خالی_خانے());
	}

	برائے (خودکار cell : pending_deleted_cells) {
		حذف cell;
	}
	pending_deleted_cells.clear();
}

خالی RTLIL::Cell::unset_پورٹ(RTLIL::IdString portname)
{
	RTLIL::SigSpec اشارہ;
	خودکار conn_it = connections_.ڈھونڈو(portname);

	اگر (conn_it != connections_.انتہا())
	{
		برائے (خودکار mon : module->monitors)
			mon->notify_connect(یہ, conn_it->پہلا, conn_it->دوسرا, اشارہ);

		اگر (module->design)
			برائے (خودکار mon : module->design->monitors)
				mon->notify_connect(یہ, conn_it->پہلا, conn_it->دوسرا, اشارہ);

		اگر (yosys_xtrace) {
			لوگارتھم("#X# Unconnect %s.%s.%s\n", یہ->module, یہ, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		اگر (module->design && module->design->flagBufferedNormalized) {
			اگر (conn_it->دوسرا.is_wire()) {
				Wire *w = conn_it->دوسرا.as_wire();
				اگر (w->driverCell_ == یہ && w->driver_پورٹ_2 == portname) {
					w->driverCell_ = خالی_اشاریہ;
					w->driver_پورٹ_2 = IdString();
					module->buf_norm_wire_قطار.داخل_کرو(w);
				} ورنہ اگر (w->driverCell_) {
					log_assert(w->driverCell_->get_پورٹ(w->driver_پورٹ_2) == w);
				}
			}

			اگر (type == ID($connect)) {
				برائے (خودکار &[پورٹ, sig] : connections_) {
					برائے (خودکار &chunk : sig.chunks()) {
						اگر (!chunk.wire)
							جاری;
						خودکار it = module->buf_norm_connect_اشاریہ.ڈھونڈو(chunk.wire);
						اگر (it == module->buf_norm_connect_اشاریہ.انتہا())
							جاری;
						it->دوسرا.erase(یہ);
						اگر (it->دوسرا.خالی_خانے())
							module->buf_norm_connect_اشاریہ.erase(it);
					}
				}
				connections_.erase(conn_it);
				برائے (خودکار &[پورٹ, sig] : connections_) {
					برائے (خودکار &chunk : sig.chunks()) {
						اگر (!chunk.wire)
							جاری;
						module->buf_norm_connect_اشاریہ[chunk.wire].داخل_کرو(یہ);
					}
				}
				واپس;
			}
		}

		connections_.erase(conn_it);
	}
}

خالی RTLIL::Cell::سیٹ_پورٹ(RTLIL::IdString portname, RTLIL::SigSpec اشارہ)
{
	خودکار r = connections_.داخل_کرو(portname);
	خودکار conn_it = r.پہلا;
	اگر (!r.دوسرا && conn_it->دوسرا == اشارہ)
		واپس;

	برائے (خودکار mon : module->monitors)
		mon->notify_connect(یہ, conn_it->پہلا, conn_it->دوسرا, اشارہ);

	اگر (module->design)
		برائے (خودکار mon : module->design->monitors)
			mon->notify_connect(یہ, conn_it->پہلا, conn_it->دوسرا, اشارہ);

	اگر (yosys_xtrace) {
		لوگارتھم("#X# Connect %s.%s.%s = %s (%d)\n", یہ->module, یہ, PooledName(module, portname).unescape(), log_signal(اشارہ), Get_سائز(اشارہ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	اگر (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		اگر (conn_it->دوسرا.is_wire()) {
			Wire *w = conn_it->دوسرا.as_wire();
			اگر (w->driverCell_ == یہ && w->driver_پورٹ_2 == portname) {
				w->driverCell_ = خالی_اشاریہ;
				w->driver_پورٹ_2 = IdString();
				module->buf_norm_wire_قطار.داخل_کرو(w);
			}
		}

		خودکار dir = پورٹ_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		اگر ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && اشارہ.is_wire()) {
			Wire *w = اشارہ.as_wire();
			اگر (w->driverCell_ == خالی_اشاریہ && (
						(w->پورٹ_داخلہ && !w->پورٹ_output) == (type == ID($داخلہ_پورٹ)))) {
				w->driverCell_ = یہ;
				w->driver_پورٹ_2 = portname;

				conn_it->دوسرا = std::move(اشارہ);
				واپس;
			}
		}

		اگر (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_قطار.داخل_کرو(یہ);
			module->buf_norm_cell_پورٹ_قطار.emplace(یہ, portname);
		} ورنہ {
			برائے (خودکار &chunk : اشارہ.chunks())
				اگر (chunk.wire != خالی_اشاریہ && chunk.wire->driverCell_ == خالی_اشاریہ)
					module->buf_norm_wire_قطار.داخل_کرو(chunk.wire);
		}

		اگر (type == ID($connect)) {
			برائے (خودکار &[پورٹ, sig] : connections_) {
				برائے (خودکار &chunk : sig.chunks()) {
					اگر (!chunk.wire)
						جاری;
					خودکار it = module->buf_norm_connect_اشاریہ.ڈھونڈو(chunk.wire);
					اگر (it == module->buf_norm_connect_اشاریہ.انتہا())
						جاری;
					it->دوسرا.erase(یہ);
					اگر (it->دوسرا.خالی_خانے())
						module->buf_norm_connect_اشاریہ.erase(it);
				}
			}
			conn_it->دوسرا = std::move(اشارہ);
			برائے (خودکار &[پورٹ, sig] : connections_) {
				برائے (خودکار &chunk : sig.chunks()) {
					اگر (!chunk.wire)
						جاری;
					module->buf_norm_connect_اشاریہ[chunk.wire].داخل_کرو(یہ);
				}
			}
			واپس;
		}
	}
	conn_it->دوسرا = std::move(اشارہ);

}

خالی RTLIL::Design::جوڑو(RTLIL::Module *module)
{
	log_assert(modules_.گنتی(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = یہ;

	برائے (خودکار mon : monitors)
		mon->notify_module_جوڑو(module);

	اگر (yosys_xtrace) {
		لوگارتھم("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	اگر (flagBufferedNormalized) {
		buf_norm_بیج_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_انتہا
