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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/modtools.h"

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <optional>

YOSYS_NAMESPACE_BEGIN


ಸ್ಥಿರ ಶೂನ್ಯ buf_norm_ಬೀಜ_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections()) {
		ಆದರೆ (Get_ಗಾತ್ರ(conn.ಎರಡನೇ) == 0 || (cell->ಪೋರ್ಟ್_dir(conn.ಮೊದಲ) != RTLIL::PD_OUTPUT && cell->ಪೋರ್ಟ್_dir(conn.ಮೊದಲ) != RTLIL::PD_INOUT))
			ಮುಂದುವರಿ;
		module->buf_norm_cell_ಸರತಿ.ಸೇರಿಸು(cell);
		module->buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ.emplace(cell, conn.ಮೊದಲ);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
		module->buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(wire);
}

ಶೂನ್ಯ RTLIL::Design::bufNormalize(ತಾರ್ಕಿಕ enable)
{
	ಆದರೆ (!enable)
	{
		ಆದರೆ (!flagBufferedNormalized)
			ಹಿಂತಿರುಗಿ;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : modules()) {
			module->buf_norm_cell_ಸರತಿ.clear();
			module->buf_norm_wire_ಸರತಿ.clear();
			module->buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ.clear();
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires()) {
				wire->driverCell_ = ಶೂನ್ಯಸೂಚಕ;
				wire->driver_ಪೋರ್ಟ್_2 = IdString();
			}
			module->buf_norm_connect_ಸೂಚ್ಯಂಕ.clear();
		}

		flagBufferedNormalized = ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (!flagBufferedNormalized)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : modules())
			buf_norm_ಬೀಜ_queues(module);

		flagBufferedNormalized = ನಿಜ;
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : modules())
		module->bufNormalize();
}

ರಚನೆ ಬಿಟ್_drive_ದತ್ತಾಂಶ_t {
	ಪೂರ್ಣಾಂಕ drivers = 0;
	ಪೂರ್ಣಾಂಕ inout = 0;
	ಪೂರ್ಣಾಂಕ users = 0;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ModWalker::ಪೋರ್ಟ್_ಬಿಟ್_2 ಪೋರ್ಟ್_ಬಿಟ್_2;

ಶೂನ್ಯ RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#ವ್ಯಾಖ್ಯಾನಿಸು xlog(...) ಮಾಡು { ಆದರೆ (yosys_xtrace) ಲಘುಗಣಕ("#X [bufnorm] " __VA_ARGS__); } ಆಗ (0)

	ಆದರೆ (!design->flagBufferedNormalized)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!buf_norm_cell_ಸರತಿ.ಖಾಲಿಗಳು() || !buf_norm_wire_ಸರತಿ.ಖಾಲಿಗಳು() || !connections_.ಖಾಲಿಗಳು())
	{
		// Ensure that every enqueued input port is represented by a cell
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : buf_norm_wire_ಸರತಿ) {
			ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು && !wire->ಪೋರ್ಟ್_output && Get_ಗಾತ್ರ(wire) != 0) {
				ಆದರೆ (wire->driverCell_ != ಶೂನ್ಯಸೂಚಕ && wire->driverCell_->type != ID($ಒಳಹರಿವು_ಪೋರ್ಟ್)) {
					wire->driverCell_ = ಶೂನ್ಯಸೂಚಕ;
					wire->driver_ಪೋರ್ಟ್_2 = IdString::Null;
				}
				ಆದರೆ (wire->driverCell_ == ಶೂನ್ಯಸೂಚಕ) {
					Cell *ಒಳಹರಿವು_ಪೋರ್ಟ್_cell = ಕೂಡಿಸು_Cell(NEW_ID, ID($ಒಳಹರಿವು_ಪೋರ್ಟ್));
					ಒಳಹರಿವು_ಪೋರ್ಟ್_cell->ಗಣ_Param(ID::ಅಗಲ_2, Get_ಗಾತ್ರ(wire));
					ಒಳಹರಿವು_ಪೋರ್ಟ್_cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
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

		idict<Wire *> wire_ಸರತಿ_entries; // Ordered queue of wires to process
		ಪೂರ್ಣಾಂಕ wire_ಸರತಿ_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> ನೇರ_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		ಸ್ವಯಂಚಾಲಿತ ಸರತಿಗೆ_ಸೇರಿಸು_cell_ಪೋರ್ಟ್ = [&](Cell *cell, IdString ಪೋರ್ಟ್) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(ಪೋರ್ಟ್));

			// An empty cell type means the cell got removed
			ಆದರೆ (cell->type.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ;


			SigSpec ಸ್ಥಿರಾಂಕ &sig = cell->get_ಪೋರ್ಟ್(ಪೋರ್ಟ್);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ chunk : sig.chunks())
				ಆದರೆ (chunk.is_wire())
					wire_ಸರತಿ_entries(chunk.wire);

			ಆದರೆ (cell->type == ID($buf) && cell->ಗುಣಲಕ್ಷಣಗಳು.ಖಾಲಿಗಳು() && !cell->name.is_ಬಹಿರಂಗ()) {
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
				log_assert(ಪೋರ್ಟ್ == ID::Y);
				SigSpec sig_a = cell->get_ಪೋರ್ಟ್(ID::A);
				SigSpec sig_y = sig;

				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &s : {sig_a, sig})
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &chunk : s.chunks())
						ಆದರೆ (chunk.wire)
							wire_ಸರತಿ_entries(chunk.wire);

				ಆದರೆ (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig_a); ++i) {
						Sig_ಬಿಟ್ b = sig_a[i];
						ಆದರೆ (b == State::Sz)
							ಮುಂದುವರಿ;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				ಆದರೆ (!sig_y.ಖಾಲಿಗಳು())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_ಗಾತ್ರ(buf_norm_wire_ಸರತಿ) <= 1);
				buf_norm_wire_ಸರತಿ.clear();
				ಹಿಂತಿರುಗಿ;
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($ಒಳಹರಿವು_ಪೋರ್ಟ್)) {
				log_assert(ಪೋರ್ಟ್ == ID::Y);
				ಆದರೆ (sig.is_wire()) {
					Wire *w = sig.as_wire();
					ಆದರೆ (w->ಪೋರ್ಟ್_ಒಳಹರಿವು && !w->ಪೋರ್ಟ್_output) {
						// An $input_port cell can only drive a full wire module input port
						ಸ್ವಯಂಚಾಲಿತ [ಸಿಕ್ಕಿತು, inserted] = ನೇರ_driven_wires.emplace(w, {cell, ಪೋರ್ಟ್});
						ಆದರೆ (!inserted || (ಸಿಕ್ಕಿತು->ಎರಡನೇ.ಮೊದಲ == cell && ಸಿಕ್ಕಿತು->ಎರಡನೇ.ಎರಡನೇ == ಪೋರ್ಟ್))
							ಹಿಂತಿರುಗಿ;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_ಸರತಿ.ಸೇರಿಸು(cell);
				remove(cell);
				log_assert(Get_ಗಾತ್ರ(buf_norm_wire_ಸರತಿ) <= 1);
				buf_norm_wire_ಸರತಿ.clear();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (sig.is_wire()) {
				Wire *w = sig.as_wire();
				ಆದರೆ (!w->ಪೋರ್ಟ್_ಒಳಹರಿವು || w->ಪೋರ್ಟ್_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					ಸ್ವಯಂಚಾಲಿತ [ಸಿಕ್ಕಿತು, inserted] = ನೇರ_driven_wires.emplace(w, {cell, ಪೋರ್ಟ್});
					ಆದರೆ (inserted || (ಸಿಕ್ಕಿತು->ಎರಡನೇ.ಮೊದಲ == cell && ಸಿಕ್ಕಿತು->ಎರಡನೇ.ಎರಡನೇ == ಪೋರ್ಟ್))
						ಹಿಂತಿರುಗಿ;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, ಪೋರ್ಟ್);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : buf_norm_wire_ಸರತಿ)
			wire_ಸರತಿ_entries(wire);
		buf_norm_wire_ಸರತಿ.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[cell, ಪೋರ್ಟ್_name] : buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ)
			ಸರತಿಗೆ_ಸೇರಿಸು_cell_ಪೋರ್ಟ್(cell, ಪೋರ್ಟ್_name);
		buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ.clear();

		// We also enqueue all wires that saw newly added module level connections.
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[a, b] : connections_)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sig : {a, b})
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &chunk : sig.chunks())
					ಆದರೆ (chunk.wire)
						wire_ಸರತಿ_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		ಆಗ (wire_ಸರತಿ_pos < Get_ಗಾತ್ರ(wire_ಸರತಿ_entries)) {
			ಸ್ವಯಂಚಾಲಿತ wire = wire_ಸರತಿ_entries[wire_ಸರತಿ_pos++];
			xlog("processing wire %s\n", wire);

			ಆದರೆ (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString ಪೋರ್ಟ್ = wire->driver_ಪೋರ್ಟ್_2;
				ಸರತಿಗೆ_ಸೇರಿಸು_cell_ಪೋರ್ಟ್(cell, ಪೋರ್ಟ್);
			}

			ಆಗ (ನಿಜ) {
				ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕಂಡುಹಿಡಿ(wire);
				ಆದರೆ (ಸಿಕ್ಕಿತು == buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕೊನೆ())
					ನಿಲ್ಲಿಸು;
				ಆಗ (!ಸಿಕ್ಕಿತು->ಎರಡನೇ.ಖಾಲಿಗಳು()) {
					Cell *connect_cell = *ಸಿಕ್ಕಿತು->ಎರಡನೇ.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ಸ್ಥಿರಾಂಕ &sig_a = connect_cell->get_ಪೋರ್ಟ್(ID::A);
					SigSpec ಸ್ಥಿರಾಂಕ &sig_b = connect_cell->get_ಪೋರ್ಟ್(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &side : {sig_a, sig_b})
						ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ chunk : side.chunks())
							ಆದರೆ (chunk.wire)
								wire_ಸರತಿ_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_ಸರತಿ.ಸೇರಿಸು(connect_cell);
					remove(connect_cell);
					log_assert(Get_ಗಾತ್ರ(buf_norm_wire_ಸರತಿ) <= 2);
					buf_norm_wire_ಸರತಿ.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[cell, ಪೋರ್ಟ್] : pending_ports) {
			log_assert(cell->type != ID($ಒಳಹರಿವು_ಪೋರ್ಟ್));
			log_assert(!cell->type.ಖಾಲಿಗಳು());
			log_assert(!pending_deleted_cells.ಎಣಿಕೆ_2(cell));
			SigSpec ಸ್ಥಿರಾಂಕ &sig = cell->get_ಪೋರ್ಟ್(ಪೋರ್ಟ್);
			Wire *w = ಕೂಡಿಸು_Wire(NEW_ID, Get_ಗಾತ್ರ(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			ನೇರ_driven_wires.emplace(w, {cell, ಪೋರ್ಟ್});
			cell->ಗಣ_ಪೋರ್ಟ್(ಪೋರ್ಟ್, w); // Hits the fast path that doesn't enqueue w
			wire_ಸರತಿ_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : wire_ಸರತಿ_entries) {
			wire->driverCell_ = ಶೂನ್ಯಸೂಚಕ;
			wire->driver_ಪೋರ್ಟ್_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[wire, cellport] : ನೇರ_driven_wires) {
			wire->driverCell_ = cellport.ಮೊದಲ;
			wire->driver_ಪೋರ್ಟ್_2 = cellport.ಎರಡನೇ;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		ಆದರೆ (yosys_xtrace)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(ಇದು);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		ಗಾಗಿ (ತಾರ್ಕಿಕ ಒಳಹರಿವು_ports : {ಸುಳ್ಳು, ನಿಜ}) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[wire, cellport] : ನೇರ_driven_wires) {
				ಆದರೆ ((wire->ಪೋರ್ಟ್_ಒಳಹರಿವು && !wire->ಪೋರ್ಟ್_output) == ಒಳಹರಿವು_ports) {
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != Get_ಗಾತ್ರ(wire); ++i) {
						Sig_ಬಿಟ್ driver = Sig_ಬಿಟ್(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_ಬಿಟ್> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_ಬಿಟ್> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_ಬಿಟ್> weakly_driven;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[wire, cellport] : ನೇರ_driven_wires) {
			ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[cell, ಪೋರ್ಟ್] = cellport;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != Get_ಗಾತ್ರ(wire); ++i) {
				Sig_ಬಿಟ್ driver = sigmap(Sig_ಬಿಟ್(wire, i));
				ಆದರೆ (cell->type == ID($tribuf) || cell->ಪೋರ್ಟ್_dir(ಪೋರ್ಟ್) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.ಸೇರಿಸು(driver);
				} ಇಲ್ಲದಿದ್ದರೆ {
					// We remove driver conflicts from `driven` in a separate loop below
					ತಾರ್ಕಿಕ inserted = driven.ಸೇರಿಸು(driver).ಎರಡನೇ;
					ಆದರೆ (!inserted)
						conflicted.ಸೇರಿಸು(driver);
				}
			}
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ driver : weakly_driven)
			driven.ಸೇರಿಸು(driver);

		// This only leaves the drivers matching `driven`'s definition above
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : wire_ಸರತಿ_entries) {
			// A zero-width wire carries no bits and needs no driver
			ಆದರೆ (Get_ಗಾತ್ರ(wire) == 0)
				ಮುಂದುವರಿ;

			SigSpec wire_drivers;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wire); ++i) {
				Sig_ಬಿಟ್ ಬಿಟ್(wire, i);
				Sig_ಬಿಟ್ mapped = sigmap(ಬಿಟ್);
				xlog("bit %s -> mapped %s\n", log_signal(ಬಿಟ್), log_signal(mapped));


				buf_connected.apply(ಬಿಟ್);
				buf_connected.ಕೂಡಿಸು(ಬಿಟ್, mapped);
				buf_connected.database.promote(mapped);

				ಆದರೆ (wire->driverCell_ == ಶೂನ್ಯಸೂಚಕ) {
					ಆದರೆ (!mapped.is_wire() || driven.ಎಣಿಕೆ_2(mapped)) {
						wire_drivers.append(mapped);
						ಮುಂದುವರಿ;
					} ಇಲ್ಲದಿದ್ದರೆ {
						wire_drivers.append(State::Sz);
					}
				}

				ಆದರೆ (ಬಿಟ್ < mapped)
					undirected_connections.emplace(ಬಿಟ್, mapped);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mapped < ಬಿಟ್)
					undirected_connections.emplace(mapped, ಬಿಟ್);
			}

			ಆದರೆ (wire->driverCell_ == ಶೂನ್ಯಸೂಚಕ) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				ಕೂಡಿಸು_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ಸ್ಥಿರ ಸ್ವಯಂಚಾಲಿತ ವಿಂಗಡಿಸು_ಕೀಲಿ = [](std::pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &p) {
			ಪೂರ್ಣಾಂಕ ಮೊದಲ_ಸ್ಥಾನಾಂತರ = p.ಮೊದಲ.is_wire() ? p.ಮೊದಲ.ಸ್ಥಾನಾಂತರ : 0;
			ಪೂರ್ಣಾಂಕ ಎರಡನೇ_ಸ್ಥಾನಾಂತರ = p.ಎರಡನೇ.is_wire() ? p.ಎರಡನೇ.ಸ್ಥಾನಾಂತರ : 0;
			ಹಿಂತಿರುಗಿ std::make_tuple(p.ಮೊದಲ.wire, p.ಎರಡನೇ.wire, ಮೊದಲ_ಸ್ಥಾನಾಂತರ - ಎರಡನೇ_ಸ್ಥಾನಾಂತರ, p);
		};

		undirected_connections.ವಿಂಗಡಿಸು([](std::pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &p, std::pair<Sig_ಬಿಟ್, Sig_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &q) {
			ಹಿಂತಿರುಗಿ ವಿಂಗಡಿಸು_ಕೀಲಿ(p) < ವಿಂಗಡಿಸು_ಕೀಲಿ(q);
		});

		SigSpec tmp_a, tmp_b;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಬಿಟ್_a, ಬಿಟ್_b] : undirected_connections) {
			tmp_a.append(ಬಿಟ್_a);
			tmp_b.append(ಬಿಟ್_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_ಬಿಟ್ ಮುಂದಿನ_a, ಮುಂದಿನ_b;

		ಸ್ವಯಂಚಾಲಿತ emit_connect_cell = [&]() {
			ಆದರೆ (sig_a.ಖಾಲಿಗಳು())
				ಹಿಂತಿರುಗಿ;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = ಕೂಡಿಸು_Cell(NEW_ID, ID($connect));
			connect_cell->ಗಣ_Param(ID::ಅಗಲ_2, Get_ಗಾತ್ರ(sig_a));
			connect_cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
			connect_cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಬಿಟ್_a, ಬಿಟ್_b] : undirected_connections) {
			ಆದರೆ (ಬಿಟ್_a == ಬಿಟ್_b)
				ಮುಂದುವರಿ;
			ಆದರೆ (ಬಿಟ್_a != ಮುಂದಿನ_a || ಬಿಟ್_b != ಮುಂದಿನ_b)
				emit_connect_cell();

			sig_a.append(ಬಿಟ್_a);
			sig_b.append(ಬಿಟ್_b);
			ಮುಂದಿನ_a = ಬಿಟ್_a;
			ಮುಂದಿನ_b = ಬಿಟ್_b;
			ಆದರೆ (ಮುಂದಿನ_a.is_wire())
				ಮುಂದಿನ_a.ಸ್ಥಾನಾಂತರ++;
			ಆದರೆ (ಮುಂದಿನ_b.is_wire())
				ಮುಂದಿನ_b.ಸ್ಥಾನಾಂತರ++;

		}
		emit_connect_cell();

		buf_norm_cell_ಸರತಿ.clear();

		log_assert(buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ.ಖಾಲಿಗಳು());
		log_assert(buf_norm_wire_ಸರತಿ.ಖಾಲಿಗಳು());
		log_assert(connections_.ಖಾಲಿಗಳು());
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : pending_deleted_cells) {
		ಅಳಿಸು cell;
	}
	pending_deleted_cells.clear();
}

ಶೂನ್ಯ RTLIL::Cell::unset_ಪೋರ್ಟ್(RTLIL::IdString portname)
{
	RTLIL::SigSpec ಸಂಕೇತ;
	ಸ್ವಯಂಚಾಲಿತ conn_it = connections_.ಕಂಡುಹಿಡಿ(portname);

	ಆದರೆ (conn_it != connections_.ಕೊನೆ())
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : module->monitors)
			mon->notify_connect(ಇದು, conn_it->ಮೊದಲ, conn_it->ಎರಡನೇ, ಸಂಕೇತ);

		ಆದರೆ (module->design)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : module->design->monitors)
				mon->notify_connect(ಇದು, conn_it->ಮೊದಲ, conn_it->ಎರಡನೇ, ಸಂಕೇತ);

		ಆದರೆ (yosys_xtrace) {
			ಲಘುಗಣಕ("#X# Unconnect %s.%s.%s\n", ಇದು->module, ಇದು, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		ಆದರೆ (module->design && module->design->flagBufferedNormalized) {
			ಆದರೆ (conn_it->ಎರಡನೇ.is_wire()) {
				Wire *w = conn_it->ಎರಡನೇ.as_wire();
				ಆದರೆ (w->driverCell_ == ಇದು && w->driver_ಪೋರ್ಟ್_2 == portname) {
					w->driverCell_ = ಶೂನ್ಯಸೂಚಕ;
					w->driver_ಪೋರ್ಟ್_2 = IdString();
					module->buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(w);
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (w->driverCell_) {
					log_assert(w->driverCell_->get_ಪೋರ್ಟ್(w->driver_ಪೋರ್ಟ್_2) == w);
				}
			}

			ಆದರೆ (type == ID($connect)) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಪೋರ್ಟ್, sig] : connections_) {
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : sig.chunks()) {
						ಆದರೆ (!chunk.wire)
							ಮುಂದುವರಿ;
						ಸ್ವಯಂಚಾಲಿತ it = module->buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕಂಡುಹಿಡಿ(chunk.wire);
						ಆದರೆ (it == module->buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕೊನೆ())
							ಮುಂದುವರಿ;
						it->ಎರಡನೇ.erase(ಇದು);
						ಆದರೆ (it->ಎರಡನೇ.ಖಾಲಿಗಳು())
							module->buf_norm_connect_ಸೂಚ್ಯಂಕ.erase(it);
					}
				}
				connections_.erase(conn_it);
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಪೋರ್ಟ್, sig] : connections_) {
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : sig.chunks()) {
						ಆದರೆ (!chunk.wire)
							ಮುಂದುವರಿ;
						module->buf_norm_connect_ಸೂಚ್ಯಂಕ[chunk.wire].ಸೇರಿಸು(ಇದು);
					}
				}
				ಹಿಂತಿರುಗಿ;
			}
		}

		connections_.erase(conn_it);
	}
}

ಶೂನ್ಯ RTLIL::Cell::ಗಣ_ಪೋರ್ಟ್(RTLIL::IdString portname, RTLIL::SigSpec ಸಂಕೇತ)
{
	ಸ್ವಯಂಚಾಲಿತ r = connections_.ಸೇರಿಸು(portname);
	ಸ್ವಯಂಚಾಲಿತ conn_it = r.ಮೊದಲ;
	ಆದರೆ (!r.ಎರಡನೇ && conn_it->ಎರಡನೇ == ಸಂಕೇತ)
		ಹಿಂತಿರುಗಿ;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : module->monitors)
		mon->notify_connect(ಇದು, conn_it->ಮೊದಲ, conn_it->ಎರಡನೇ, ಸಂಕೇತ);

	ಆದರೆ (module->design)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : module->design->monitors)
			mon->notify_connect(ಇದು, conn_it->ಮೊದಲ, conn_it->ಎರಡನೇ, ಸಂಕೇತ);

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# Connect %s.%s.%s = %s (%d)\n", ಇದು->module, ಇದು, PooledName(module, portname).unescape(), log_signal(ಸಂಕೇತ), Get_ಗಾತ್ರ(ಸಂಕೇತ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ಆದರೆ (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		ಆದರೆ (conn_it->ಎರಡನೇ.is_wire()) {
			Wire *w = conn_it->ಎರಡನೇ.as_wire();
			ಆದರೆ (w->driverCell_ == ಇದು && w->driver_ಪೋರ್ಟ್_2 == portname) {
				w->driverCell_ = ಶೂನ್ಯಸೂಚಕ;
				w->driver_ಪೋರ್ಟ್_2 = IdString();
				module->buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(w);
			}
		}

		ಸ್ವಯಂಚಾಲಿತ dir = ಪೋರ್ಟ್_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		ಆದರೆ ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && ಸಂಕೇತ.is_wire()) {
			Wire *w = ಸಂಕೇತ.as_wire();
			ಆದರೆ (w->driverCell_ == ಶೂನ್ಯಸೂಚಕ && (
						(w->ಪೋರ್ಟ್_ಒಳಹರಿವು && !w->ಪೋರ್ಟ್_output) == (type == ID($ಒಳಹರಿವು_ಪೋರ್ಟ್)))) {
				w->driverCell_ = ಇದು;
				w->driver_ಪೋರ್ಟ್_2 = portname;

				conn_it->ಎರಡನೇ = std::move(ಸಂಕೇತ);
				ಹಿಂತಿರುಗಿ;
			}
		}

		ಆದರೆ (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_ಸರತಿ.ಸೇರಿಸು(ಇದು);
			module->buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ.emplace(ಇದು, portname);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : ಸಂಕೇತ.chunks())
				ಆದರೆ (chunk.wire != ಶೂನ್ಯಸೂಚಕ && chunk.wire->driverCell_ == ಶೂನ್ಯಸೂಚಕ)
					module->buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(chunk.wire);
		}

		ಆದರೆ (type == ID($connect)) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಪೋರ್ಟ್, sig] : connections_) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : sig.chunks()) {
					ಆದರೆ (!chunk.wire)
						ಮುಂದುವರಿ;
					ಸ್ವಯಂಚಾಲಿತ it = module->buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕಂಡುಹಿಡಿ(chunk.wire);
					ಆದರೆ (it == module->buf_norm_connect_ಸೂಚ್ಯಂಕ.ಕೊನೆ())
						ಮುಂದುವರಿ;
					it->ಎರಡನೇ.erase(ಇದು);
					ಆದರೆ (it->ಎರಡನೇ.ಖಾಲಿಗಳು())
						module->buf_norm_connect_ಸೂಚ್ಯಂಕ.erase(it);
				}
			}
			conn_it->ಎರಡನೇ = std::move(ಸಂಕೇತ);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[ಪೋರ್ಟ್, sig] : connections_) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : sig.chunks()) {
					ಆದರೆ (!chunk.wire)
						ಮುಂದುವರಿ;
					module->buf_norm_connect_ಸೂಚ್ಯಂಕ[chunk.wire].ಸೇರಿಸು(ಇದು);
				}
			}
			ಹಿಂತಿರುಗಿ;
		}
	}
	conn_it->ಎರಡನೇ = std::move(ಸಂಕೇತ);

}

ಶೂನ್ಯ RTLIL::Design::ಕೂಡಿಸು(RTLIL::Module *module)
{
	log_assert(modules_.ಎಣಿಕೆ_2(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = ಇದು;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : monitors)
		mon->notify_module_ಕೂಡಿಸು(module);

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ಆದರೆ (flagBufferedNormalized) {
		buf_norm_ಬೀಜ_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_ಕೊನೆ
