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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/modtools.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>

YOSYS_NAMESPACE_BEGIN


ਸਥਿਰ ਖਾਲੀ buf_norm_ਬੀਜ_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
	ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections()) {
		ਜੇ (Get_ਮਾਪ(conn.ਦੂਜਾ) == 0 || (cell->ਪੋਰਟ_dir(conn.ਪਹਿਲਾ) != RTLIL::PD_OUTPUT && cell->ਪੋਰਟ_dir(conn.ਪਹਿਲਾ) != RTLIL::PD_INOUT))
			ਜਾਰੀ;
		module->buf_norm_cell_ਕਤਾਰ.ਪਾਓ(cell);
		module->buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ.emplace(cell, conn.ਪਹਿਲਾ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
		module->buf_norm_wire_ਕਤਾਰ.ਪਾਓ(wire);
}

ਖਾਲੀ RTLIL::Design::bufNormalize(ਤਾਰਕਿਕ enable)
{
	ਜੇ (!enable)
	{
		ਜੇ (!flagBufferedNormalized)
			ਵਾਪਸ;

		ਲਈ (ਸਵੈਚਾਲਿਤ module : modules()) {
			module->buf_norm_cell_ਕਤਾਰ.clear();
			module->buf_norm_wire_ਕਤਾਰ.clear();
			module->buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ.clear();
			ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires()) {
				wire->driverCell_ = ਖਾਲੀ_ਸੂਚਕ;
				wire->driver_ਪੋਰਟ_2 = IdString();
			}
			module->buf_norm_connect_ਸੂਚਕਾਂਕ.clear();
		}

		flagBufferedNormalized = ਝੂਠ;
		ਵਾਪਸ;
	}

	ਜੇ (!flagBufferedNormalized)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ module : modules())
			buf_norm_ਬੀਜ_queues(module);

		flagBufferedNormalized = ਸੱਚ;
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ module : modules())
		module->bufNormalize();
}

ਬਣਤਰ ਦੋਆਧਾਰੀ_ਅੰਕ_drive_ਅੰਕੜੇ_t {
	ਪੂਰਨ drivers = 0;
	ਪੂਰਨ inout = 0;
	ਪੂਰਨ users = 0;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ModWalker::ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2 ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2;

ਖਾਲੀ RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ xlog(...) ਕਰੋ { ਜੇ (yosys_xtrace) ਲਘੂਗਣਕ("#X [bufnorm] " __VA_ARGS__); } ਜਦੋਂ (0)

	ਜੇ (!design->flagBufferedNormalized)
		ਵਾਪਸ;

	ਜੇ (!buf_norm_cell_ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ() || !buf_norm_wire_ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ() || !connections_.ਖਾਲੀ_ਥਾਵਾਂ())
	{
		// Ensure that every enqueued input port is represented by a cell
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : buf_norm_wire_ਕਤਾਰ) {
			ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼ && !wire->ਪੋਰਟ_output && Get_ਮਾਪ(wire) != 0) {
				ਜੇ (wire->driverCell_ != ਖਾਲੀ_ਸੂਚਕ && wire->driverCell_->type != ID($ਨਿਵੇਸ਼_ਪੋਰਟ)) {
					wire->driverCell_ = ਖਾਲੀ_ਸੂਚਕ;
					wire->driver_ਪੋਰਟ_2 = IdString::Null;
				}
				ਜੇ (wire->driverCell_ == ਖਾਲੀ_ਸੂਚਕ) {
					Cell *ਨਿਵੇਸ਼_ਪੋਰਟ_cell = ਜੋੜੋ_Cell(NEW_ID, ID($ਨਿਵੇਸ਼_ਪੋਰਟ));
					ਨਿਵੇਸ਼_ਪੋਰਟ_cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, Get_ਮਾਪ(wire));
					ਨਿਵੇਸ਼_ਪੋਰਟ_cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
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

		idict<Wire *> wire_ਕਤਾਰ_entries; // Ordered queue of wires to process
		ਪੂਰਨ wire_ਕਤਾਰ_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> ਸਿੱਧਾ_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		ਸਵੈਚਾਲਿਤ ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_cell_ਪੋਰਟ = [&](Cell *cell, IdString ਪੋਰਟ) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(ਪੋਰਟ));

			// An empty cell type means the cell got removed
			ਜੇ (cell->type.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ;


			SigSpec ਅਚਲ &sig = cell->get_ਪੋਰਟ(ਪੋਰਟ);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			ਲਈ (ਸਵੈਚਾਲਿਤ chunk : sig.chunks())
				ਜੇ (chunk.is_wire())
					wire_ਕਤਾਰ_entries(chunk.wire);

			ਜੇ (cell->type == ID($buf) && cell->ਗੁਣ.ਖਾਲੀ_ਥਾਵਾਂ() && !cell->name.is_ਜਨਤਕ()) {
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
				log_assert(ਪੋਰਟ == ID::Y);
				SigSpec sig_a = cell->get_ਪੋਰਟ(ID::A);
				SigSpec sig_y = sig;

				ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &s : {sig_a, sig})
					ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &chunk : s.chunks())
						ਜੇ (chunk.wire)
							wire_ਕਤਾਰ_entries(chunk.wire);

				ਜੇ (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig_a); ++i) {
						Sig_ਦੋਆਧਾਰੀ_ਅੰਕ b = sig_a[i];
						ਜੇ (b == State::Sz)
							ਜਾਰੀ;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				ਜੇ (!sig_y.ਖਾਲੀ_ਥਾਵਾਂ())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_ਮਾਪ(buf_norm_wire_ਕਤਾਰ) <= 1);
				buf_norm_wire_ਕਤਾਰ.clear();
				ਵਾਪਸ;
			} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($ਨਿਵੇਸ਼_ਪੋਰਟ)) {
				log_assert(ਪੋਰਟ == ID::Y);
				ਜੇ (sig.is_wire()) {
					Wire *w = sig.as_wire();
					ਜੇ (w->ਪੋਰਟ_ਨਿਵੇਸ਼ && !w->ਪੋਰਟ_output) {
						// An $input_port cell can only drive a full wire module input port
						ਸਵੈਚਾਲਿਤ [ਮਿਲਿਆ, inserted] = ਸਿੱਧਾ_driven_wires.emplace(w, {cell, ਪੋਰਟ});
						ਜੇ (!inserted || (ਮਿਲਿਆ->ਦੂਜਾ.ਪਹਿਲਾ == cell && ਮਿਲਿਆ->ਦੂਜਾ.ਦੂਜਾ == ਪੋਰਟ))
							ਵਾਪਸ;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_ਕਤਾਰ.ਪਾਓ(cell);
				remove(cell);
				log_assert(Get_ਮਾਪ(buf_norm_wire_ਕਤਾਰ) <= 1);
				buf_norm_wire_ਕਤਾਰ.clear();
				ਵਾਪਸ;
			}

			ਜੇ (sig.is_wire()) {
				Wire *w = sig.as_wire();
				ਜੇ (!w->ਪੋਰਟ_ਨਿਵੇਸ਼ || w->ਪੋਰਟ_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					ਸਵੈਚਾਲਿਤ [ਮਿਲਿਆ, inserted] = ਸਿੱਧਾ_driven_wires.emplace(w, {cell, ਪੋਰਟ});
					ਜੇ (inserted || (ਮਿਲਿਆ->ਦੂਜਾ.ਪਹਿਲਾ == cell && ਮਿਲਿਆ->ਦੂਜਾ.ਦੂਜਾ == ਪੋਰਟ))
						ਵਾਪਸ;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, ਪੋਰਟ);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : buf_norm_wire_ਕਤਾਰ)
			wire_ਕਤਾਰ_entries(wire);
		buf_norm_wire_ਕਤਾਰ.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &[cell, ਪੋਰਟ_name] : buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ)
			ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_cell_ਪੋਰਟ(cell, ਪੋਰਟ_name);
		buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ.clear();

		// We also enqueue all wires that saw newly added module level connections.
		ਲਈ (ਸਵੈਚਾਲਿਤ &[a, b] : connections_)
			ਲਈ (ਸਵੈਚਾਲਿਤ &sig : {a, b})
				ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &chunk : sig.chunks())
					ਜੇ (chunk.wire)
						wire_ਕਤਾਰ_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		ਜਦੋਂ (wire_ਕਤਾਰ_pos < Get_ਮਾਪ(wire_ਕਤਾਰ_entries)) {
			ਸਵੈਚਾਲਿਤ wire = wire_ਕਤਾਰ_entries[wire_ਕਤਾਰ_pos++];
			xlog("processing wire %s\n", wire);

			ਜੇ (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString ਪੋਰਟ = wire->driver_ਪੋਰਟ_2;
				ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_cell_ਪੋਰਟ(cell, ਪੋਰਟ);
			}

			ਜਦੋਂ (ਸੱਚ) {
				ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = buf_norm_connect_ਸੂਚਕਾਂਕ.ਲੱਭੋ(wire);
				ਜੇ (ਮਿਲਿਆ == buf_norm_connect_ਸੂਚਕਾਂਕ.ਸਿਰਾ())
					ਰੋਕੋ;
				ਜਦੋਂ (!ਮਿਲਿਆ->ਦੂਜਾ.ਖਾਲੀ_ਥਾਵਾਂ()) {
					Cell *connect_cell = *ਮਿਲਿਆ->ਦੂਜਾ.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ਅਚਲ &sig_a = connect_cell->get_ਪੋਰਟ(ID::A);
					SigSpec ਅਚਲ &sig_b = connect_cell->get_ਪੋਰਟ(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					ਲਈ (ਸਵੈਚਾਲਿਤ &side : {sig_a, sig_b})
						ਲਈ (ਸਵੈਚਾਲਿਤ chunk : side.chunks())
							ਜੇ (chunk.wire)
								wire_ਕਤਾਰ_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_ਕਤਾਰ.ਪਾਓ(connect_cell);
					remove(connect_cell);
					log_assert(Get_ਮਾਪ(buf_norm_wire_ਕਤਾਰ) <= 2);
					buf_norm_wire_ਕਤਾਰ.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		ਲਈ (ਸਵੈਚਾਲਿਤ &[cell, ਪੋਰਟ] : pending_ports) {
			log_assert(cell->type != ID($ਨਿਵੇਸ਼_ਪੋਰਟ));
			log_assert(!cell->type.ਖਾਲੀ_ਥਾਵਾਂ());
			log_assert(!pending_deleted_cells.ਗਿਣਤੀ_2(cell));
			SigSpec ਅਚਲ &sig = cell->get_ਪੋਰਟ(ਪੋਰਟ);
			Wire *w = ਜੋੜੋ_Wire(NEW_ID, Get_ਮਾਪ(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			ਸਿੱਧਾ_driven_wires.emplace(w, {cell, ਪੋਰਟ});
			cell->ਸੈੱਟ_ਪੋਰਟ(ਪੋਰਟ, w); // Hits the fast path that doesn't enqueue w
			wire_ਕਤਾਰ_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : wire_ਕਤਾਰ_entries) {
			wire->driverCell_ = ਖਾਲੀ_ਸੂਚਕ;
			wire->driver_ਪੋਰਟ_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &[wire, cellport] : ਸਿੱਧਾ_driven_wires) {
			wire->driverCell_ = cellport.ਪਹਿਲਾ;
			wire->driver_ਪੋਰਟ_2 = cellport.ਦੂਜਾ;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		ਜੇ (yosys_xtrace)
			ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(ਇਹ);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		ਲਈ (ਤਾਰਕਿਕ ਨਿਵੇਸ਼_ports : {ਝੂਠ, ਸੱਚ}) {
			ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &[wire, cellport] : ਸਿੱਧਾ_driven_wires) {
				ਜੇ ((wire->ਪੋਰਟ_ਨਿਵੇਸ਼ && !wire->ਪੋਰਟ_output) == ਨਿਵੇਸ਼_ports) {
					ਲਈ (ਪੂਰਨ i = 0; i != Get_ਮਾਪ(wire); ++i) {
						Sig_ਦੋਆਧਾਰੀ_ਅੰਕ driver = Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> weakly_driven;

		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &[wire, cellport] : ਸਿੱਧਾ_driven_wires) {
			ਸਵੈਚਾਲਿਤ ਅਚਲ &[cell, ਪੋਰਟ] = cellport;
			ਲਈ (ਪੂਰਨ i = 0; i != Get_ਮਾਪ(wire); ++i) {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ driver = sigmap(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i));
				ਜੇ (cell->type == ID($tribuf) || cell->ਪੋਰਟ_dir(ਪੋਰਟ) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.ਪਾਓ(driver);
				} ਨਹੀਂਤਾਂ {
					// We remove driver conflicts from `driven` in a separate loop below
					ਤਾਰਕਿਕ inserted = driven.ਪਾਓ(driver).ਦੂਜਾ;
					ਜੇ (!inserted)
						conflicted.ਪਾਓ(driver);
				}
			}
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ driver : weakly_driven)
			driven.ਪਾਓ(driver);

		// This only leaves the drivers matching `driven`'s definition above
		ਲਈ (ਸਵੈਚਾਲਿਤ driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : wire_ਕਤਾਰ_entries) {
			// A zero-width wire carries no bits and needs no driver
			ਜੇ (Get_ਮਾਪ(wire) == 0)
				ਜਾਰੀ;

			SigSpec wire_drivers;
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wire); ++i) {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i);
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ mapped = sigmap(ਦੋਆਧਾਰੀ_ਅੰਕ);
				xlog("bit %s -> mapped %s\n", log_signal(ਦੋਆਧਾਰੀ_ਅੰਕ), log_signal(mapped));


				buf_connected.apply(ਦੋਆਧਾਰੀ_ਅੰਕ);
				buf_connected.ਜੋੜੋ(ਦੋਆਧਾਰੀ_ਅੰਕ, mapped);
				buf_connected.database.promote(mapped);

				ਜੇ (wire->driverCell_ == ਖਾਲੀ_ਸੂਚਕ) {
					ਜੇ (!mapped.is_wire() || driven.ਗਿਣਤੀ_2(mapped)) {
						wire_drivers.append(mapped);
						ਜਾਰੀ;
					} ਨਹੀਂਤਾਂ {
						wire_drivers.append(State::Sz);
					}
				}

				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ < mapped)
					undirected_connections.emplace(ਦੋਆਧਾਰੀ_ਅੰਕ, mapped);
				ਨਹੀਂਤਾਂ ਜੇ (mapped < ਦੋਆਧਾਰੀ_ਅੰਕ)
					undirected_connections.emplace(mapped, ਦੋਆਧਾਰੀ_ਅੰਕ);
			}

			ਜੇ (wire->driverCell_ == ਖਾਲੀ_ਸੂਚਕ) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				ਜੋੜੋ_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ਸਥਿਰ ਸਵੈਚਾਲਿਤ ਛਾਂਟੋ_ਕੁੰਜੀ = [](std::pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &p) {
			ਪੂਰਨ ਪਹਿਲਾ_ਵਿੱਥ = p.ਪਹਿਲਾ.is_wire() ? p.ਪਹਿਲਾ.ਵਿੱਥ : 0;
			ਪੂਰਨ ਦੂਜਾ_ਵਿੱਥ = p.ਦੂਜਾ.is_wire() ? p.ਦੂਜਾ.ਵਿੱਥ : 0;
			ਵਾਪਸ std::make_tuple(p.ਪਹਿਲਾ.wire, p.ਦੂਜਾ.wire, ਪਹਿਲਾ_ਵਿੱਥ - ਦੂਜਾ_ਵਿੱਥ, p);
		};

		undirected_connections.ਛਾਂਟੋ([](std::pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &p, std::pair<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &q) {
			ਵਾਪਸ ਛਾਂਟੋ_ਕੁੰਜੀ(p) < ਛਾਂਟੋ_ਕੁੰਜੀ(q);
		});

		SigSpec tmp_a, tmp_b;

		ਲਈ (ਸਵੈਚਾਲਿਤ &[ਦੋਆਧਾਰੀ_ਅੰਕ_a, ਦੋਆਧਾਰੀ_ਅੰਕ_b] : undirected_connections) {
			tmp_a.append(ਦੋਆਧਾਰੀ_ਅੰਕ_a);
			tmp_b.append(ਦੋਆਧਾਰੀ_ਅੰਕ_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਗਲਾ_a, ਅਗਲਾ_b;

		ਸਵੈਚਾਲਿਤ emit_connect_cell = [&]() {
			ਜੇ (sig_a.ਖਾਲੀ_ਥਾਵਾਂ())
				ਵਾਪਸ;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = ਜੋੜੋ_Cell(NEW_ID, ID($connect));
			connect_cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, Get_ਮਾਪ(sig_a));
			connect_cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
			connect_cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		ਲਈ (ਸਵੈਚਾਲਿਤ &[ਦੋਆਧਾਰੀ_ਅੰਕ_a, ਦੋਆਧਾਰੀ_ਅੰਕ_b] : undirected_connections) {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_a == ਦੋਆਧਾਰੀ_ਅੰਕ_b)
				ਜਾਰੀ;
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_a != ਅਗਲਾ_a || ਦੋਆਧਾਰੀ_ਅੰਕ_b != ਅਗਲਾ_b)
				emit_connect_cell();

			sig_a.append(ਦੋਆਧਾਰੀ_ਅੰਕ_a);
			sig_b.append(ਦੋਆਧਾਰੀ_ਅੰਕ_b);
			ਅਗਲਾ_a = ਦੋਆਧਾਰੀ_ਅੰਕ_a;
			ਅਗਲਾ_b = ਦੋਆਧਾਰੀ_ਅੰਕ_b;
			ਜੇ (ਅਗਲਾ_a.is_wire())
				ਅਗਲਾ_a.ਵਿੱਥ++;
			ਜੇ (ਅਗਲਾ_b.is_wire())
				ਅਗਲਾ_b.ਵਿੱਥ++;

		}
		emit_connect_cell();

		buf_norm_cell_ਕਤਾਰ.clear();

		log_assert(buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ());
		log_assert(buf_norm_wire_ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ());
		log_assert(connections_.ਖਾਲੀ_ਥਾਵਾਂ());
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ cell : pending_deleted_cells) {
		ਮਿਟਾਓ cell;
	}
	pending_deleted_cells.clear();
}

ਖਾਲੀ RTLIL::Cell::unset_ਪੋਰਟ(RTLIL::IdString portname)
{
	RTLIL::SigSpec ਸੰਕੇਤ;
	ਸਵੈਚਾਲਿਤ conn_it = connections_.ਲੱਭੋ(portname);

	ਜੇ (conn_it != connections_.ਸਿਰਾ())
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ mon : module->monitors)
			mon->notify_connect(ਇਹ, conn_it->ਪਹਿਲਾ, conn_it->ਦੂਜਾ, ਸੰਕੇਤ);

		ਜੇ (module->design)
			ਲਈ (ਸਵੈਚਾਲਿਤ mon : module->design->monitors)
				mon->notify_connect(ਇਹ, conn_it->ਪਹਿਲਾ, conn_it->ਦੂਜਾ, ਸੰਕੇਤ);

		ਜੇ (yosys_xtrace) {
			ਲਘੂਗਣਕ("#X# Unconnect %s.%s.%s\n", ਇਹ->module, ਇਹ, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		ਜੇ (module->design && module->design->flagBufferedNormalized) {
			ਜੇ (conn_it->ਦੂਜਾ.is_wire()) {
				Wire *w = conn_it->ਦੂਜਾ.as_wire();
				ਜੇ (w->driverCell_ == ਇਹ && w->driver_ਪੋਰਟ_2 == portname) {
					w->driverCell_ = ਖਾਲੀ_ਸੂਚਕ;
					w->driver_ਪੋਰਟ_2 = IdString();
					module->buf_norm_wire_ਕਤਾਰ.ਪਾਓ(w);
				} ਨਹੀਂਤਾਂ ਜੇ (w->driverCell_) {
					log_assert(w->driverCell_->get_ਪੋਰਟ(w->driver_ਪੋਰਟ_2) == w);
				}
			}

			ਜੇ (type == ID($connect)) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &[ਪੋਰਟ, sig] : connections_) {
					ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : sig.chunks()) {
						ਜੇ (!chunk.wire)
							ਜਾਰੀ;
						ਸਵੈਚਾਲਿਤ it = module->buf_norm_connect_ਸੂਚਕਾਂਕ.ਲੱਭੋ(chunk.wire);
						ਜੇ (it == module->buf_norm_connect_ਸੂਚਕਾਂਕ.ਸਿਰਾ())
							ਜਾਰੀ;
						it->ਦੂਜਾ.erase(ਇਹ);
						ਜੇ (it->ਦੂਜਾ.ਖਾਲੀ_ਥਾਵਾਂ())
							module->buf_norm_connect_ਸੂਚਕਾਂਕ.erase(it);
					}
				}
				connections_.erase(conn_it);
				ਲਈ (ਸਵੈਚਾਲਿਤ &[ਪੋਰਟ, sig] : connections_) {
					ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : sig.chunks()) {
						ਜੇ (!chunk.wire)
							ਜਾਰੀ;
						module->buf_norm_connect_ਸੂਚਕਾਂਕ[chunk.wire].ਪਾਓ(ਇਹ);
					}
				}
				ਵਾਪਸ;
			}
		}

		connections_.erase(conn_it);
	}
}

ਖਾਲੀ RTLIL::Cell::ਸੈੱਟ_ਪੋਰਟ(RTLIL::IdString portname, RTLIL::SigSpec ਸੰਕੇਤ)
{
	ਸਵੈਚਾਲਿਤ r = connections_.ਪਾਓ(portname);
	ਸਵੈਚਾਲਿਤ conn_it = r.ਪਹਿਲਾ;
	ਜੇ (!r.ਦੂਜਾ && conn_it->ਦੂਜਾ == ਸੰਕੇਤ)
		ਵਾਪਸ;

	ਲਈ (ਸਵੈਚਾਲਿਤ mon : module->monitors)
		mon->notify_connect(ਇਹ, conn_it->ਪਹਿਲਾ, conn_it->ਦੂਜਾ, ਸੰਕੇਤ);

	ਜੇ (module->design)
		ਲਈ (ਸਵੈਚਾਲਿਤ mon : module->design->monitors)
			mon->notify_connect(ਇਹ, conn_it->ਪਹਿਲਾ, conn_it->ਦੂਜਾ, ਸੰਕੇਤ);

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# Connect %s.%s.%s = %s (%d)\n", ਇਹ->module, ਇਹ, PooledName(module, portname).unescape(), log_signal(ਸੰਕੇਤ), Get_ਮਾਪ(ਸੰਕੇਤ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ਜੇ (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		ਜੇ (conn_it->ਦੂਜਾ.is_wire()) {
			Wire *w = conn_it->ਦੂਜਾ.as_wire();
			ਜੇ (w->driverCell_ == ਇਹ && w->driver_ਪੋਰਟ_2 == portname) {
				w->driverCell_ = ਖਾਲੀ_ਸੂਚਕ;
				w->driver_ਪੋਰਟ_2 = IdString();
				module->buf_norm_wire_ਕਤਾਰ.ਪਾਓ(w);
			}
		}

		ਸਵੈਚਾਲਿਤ dir = ਪੋਰਟ_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		ਜੇ ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && ਸੰਕੇਤ.is_wire()) {
			Wire *w = ਸੰਕੇਤ.as_wire();
			ਜੇ (w->driverCell_ == ਖਾਲੀ_ਸੂਚਕ && (
						(w->ਪੋਰਟ_ਨਿਵੇਸ਼ && !w->ਪੋਰਟ_output) == (type == ID($ਨਿਵੇਸ਼_ਪੋਰਟ)))) {
				w->driverCell_ = ਇਹ;
				w->driver_ਪੋਰਟ_2 = portname;

				conn_it->ਦੂਜਾ = std::move(ਸੰਕੇਤ);
				ਵਾਪਸ;
			}
		}

		ਜੇ (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_ਕਤਾਰ.ਪਾਓ(ਇਹ);
			module->buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ.emplace(ਇਹ, portname);
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : ਸੰਕੇਤ.chunks())
				ਜੇ (chunk.wire != ਖਾਲੀ_ਸੂਚਕ && chunk.wire->driverCell_ == ਖਾਲੀ_ਸੂਚਕ)
					module->buf_norm_wire_ਕਤਾਰ.ਪਾਓ(chunk.wire);
		}

		ਜੇ (type == ID($connect)) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &[ਪੋਰਟ, sig] : connections_) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : sig.chunks()) {
					ਜੇ (!chunk.wire)
						ਜਾਰੀ;
					ਸਵੈਚਾਲਿਤ it = module->buf_norm_connect_ਸੂਚਕਾਂਕ.ਲੱਭੋ(chunk.wire);
					ਜੇ (it == module->buf_norm_connect_ਸੂਚਕਾਂਕ.ਸਿਰਾ())
						ਜਾਰੀ;
					it->ਦੂਜਾ.erase(ਇਹ);
					ਜੇ (it->ਦੂਜਾ.ਖਾਲੀ_ਥਾਵਾਂ())
						module->buf_norm_connect_ਸੂਚਕਾਂਕ.erase(it);
				}
			}
			conn_it->ਦੂਜਾ = std::move(ਸੰਕੇਤ);
			ਲਈ (ਸਵੈਚਾਲਿਤ &[ਪੋਰਟ, sig] : connections_) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : sig.chunks()) {
					ਜੇ (!chunk.wire)
						ਜਾਰੀ;
					module->buf_norm_connect_ਸੂਚਕਾਂਕ[chunk.wire].ਪਾਓ(ਇਹ);
				}
			}
			ਵਾਪਸ;
		}
	}
	conn_it->ਦੂਜਾ = std::move(ਸੰਕੇਤ);

}

ਖਾਲੀ RTLIL::Design::ਜੋੜੋ(RTLIL::Module *module)
{
	log_assert(modules_.ਗਿਣਤੀ_2(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = ਇਹ;

	ਲਈ (ਸਵੈਚਾਲਿਤ mon : monitors)
		mon->notify_module_ਜੋੜੋ(module);

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ਜੇ (flagBufferedNormalized) {
		buf_norm_ਬੀਜ_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_ਸਿਰਾ
